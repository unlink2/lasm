#include "test_frontend.h"

#include "frontend.h"
#include "macros.h"
#include "instruction6502.h"
#include "instruction65816.h"

using namespace lasm;

class DummyReader: public FileReader {
    public:
        DummyReader(std::string filecontent):
            content(std::make_shared<std::istringstream>(std::istringstream(filecontent))) {}

        virtual std::shared_ptr<std::istream> openFile(std::string fromPath) {
            if (fromPath == "inc.asm") {
                return std::make_shared<std::istringstream>(std::istringstream("lda #0xFF;\nincluded_label:\nnop;"));
            } else if (fromPath == "inc.bin") {
                return std::make_shared<std::istringstream>(std::istringstream("Hello"));
            }
            return content;
        }
    private:
        std::shared_ptr<std::istringstream> content;
};

class DummyWriter: public FileWriter {
    public:
        virtual std::shared_ptr<std::ostream> openFile(std::string fromPath) {
            if (fromPath == "test.lst") {
                return list;
            }

            return bin;
        }
        std::shared_ptr<std::ostringstream> list = std::make_shared<std::ostringstream>(std::ostringstream());
        std::shared_ptr<std::ostringstream> bin = std::make_shared<std::ostringstream>(std::ostringstream());
};

#define test_full(code, lst, is, ...) {\
    auto reader = DummyReader(code);\
    auto writer = DummyWriter();\
    is instructions;\
    Frontend frontend(instructions, reader, writer);\
    frontend.assemble("test.asm", "test.bin", "test.lst");\
    char dataArray[] = __VA_ARGS__;\
    assert_cc_string_equal(writer.list->str(), std::string(lst));\
    assert_memory_equal(dataArray, writer.bin->str().c_str(), writer.bin->str().length());\
}

#define test_full_err(code, is, errorCode) {\
    auto reader = DummyReader(code);\
    auto writer = DummyWriter();\
    is instructions;\
    std::stringstream nopstream;\
    Frontend frontend(instructions, reader, writer, Frontend::defaultSettings, nopstream);\
    assert_int_equal(frontend.assemble("test.asm", "test.bin", "test.lst"), errorCode);\
}

void test_frontend(void **state) {
    test_full("adc #0xFF;\n"
            "test: let j = 20;"
            "let i = 100;\n"
            "cmp i;",

            "test = 0x2\n"
            "i = 0x64\n"
            "j = 0x14\n",
            InstructionSet6502,
            {0x69, (char)0xFF, (char)0xC5, (char)0x64});


    // test include and incbin
    test_full("org 0x8000; nop; include \"inc.asm\"\nnop;\nincbin \"inc.bin\"\nnop; db ord('a'), len(\"Hello\"),"
            "len([1, 2, 3]);",
            "included_label = 0x8003\n",
            InstructionSet6502,
            {(char)0xEA, (char)0xA9, (char)0xFF, (char)0xEA, (char)0xEA,
            'H', 'e', 'l', 'l', 'o', (char)0xEA, 'a', 0x05, 0x03});

    // test label names
    test_full("org 0x8000;\n"
            "scope1: {\n"
            "setScopeName(\"scopeName\");"
                "sublabel: {\nnop;\n"
                "}\n}",
            "scope1 = 0x8000\nscopeName.sublabel = 0x8000\n",
            InstructionSet6502,
            {(char)0xEA});

    // test 65816 immediate16, long and long, x
    // sr, src,x block move
    test_full("m16; adc #0xFFFF;\n"
            "adc 0x1FFFF;"
            "adc 0x1FFAA, x;"
            "adc 0x1A, s;"
            "adc (0x1A, s), y;"
            "adc [0x11];"
            "adc [0x12], y;"
            "mvn 0x1, 0x2;\n"
            "brl _A()+0x1AB;\n"
            "adc (0x1F);"
            "jml 0x1234;"
            "jsl 0x5678;"
            "jsr (0x1234,x);"
            "jmp (0x1234, x);"
            "pea 0x1232;"
            "pei (0x12);"
            "rep #0x33;"
            "adc.z 0x1A;"
            "adc.w 0x1A;"
            "adc.l +0x1A;",

            "",
            InstructionSet65816,
            {0x69, (char)0xFF, (char)0xFF,
            0x6F, (char)0xFF, (char)0xFF, (char)0x01,
            0x7F, (char)0xAA, (char)0xFF, (char)0x01,
            0x63, (char)0x1A,
            0x73, (char)0x1A,
            0x67, (char)0x11,
            0x77, (char)0x12,
            0x54, (char)0x02, (char)0x01,
            (char)0x82, (char)0xA8, (char)0x01,
            0x72, 0x1F,
            0x5C, 0x34, 0x12, 0x00,
            0x22, 0x78, 0x56, 0x00,
            (char)0xFC, (char)0x34, (char)0x12,
            0x7C, 0x34, 0x12,
            (char)0xF4, 0x32, (char)0x12,
            (char)0xD4, 0x12,
            (char)0xC2, 0x33,
            (char)0x65, 0x1A,
            (char)0x6D, 0x1A, 0x00,
            (char)0x6F, 0x1A, 0x00, 0x00});

}

void test_frontend_errors(void **state) {
    // test errors
    test_full_err("noo;", InstructionSet6502, UNDEFINED_REF);
    test_full_err("adc [0x11], x;", InstructionSet65816, INVALID_INSTRUCTION);
    test_full_err("adc [0x1112];", InstructionSet65816, VALUE_OUT_OF_RANGE);

    // block move errors
    test_full_err("mvp 0x01;", InstructionSet65816, MISSING_COMMA);
    test_full_err("mvp 'hi', 0x01;", InstructionSet65816, TYPE_ERROR);
    test_full_err("mvp 0x01, 'test';", InstructionSet65816, TYPE_ERROR);
    test_full_err("mvp 0x01, 0x256;", InstructionSet65816, VALUE_OUT_OF_RANGE);
    test_full_err("mvp 0x256, 0x01;", InstructionSet65816, VALUE_OUT_OF_RANGE);
    test_full_err("brl 32772;", InstructionSet65816, VALUE_OUT_OF_RANGE);
    test_full_err("adc (0x1f1f);", InstructionSet65816, VALUE_OUT_OF_RANGE);
    test_full_err("adc.i 0x1A;", InstructionSet65816, INVALID_INSTRUCTION);
}
