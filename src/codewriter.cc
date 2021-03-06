#include "codewriter.h"

namespace lasm {
    void BinaryWriter::write(std::string path) {
        auto os = writer.openFile(path);
        for (auto b : binary) {
            os->write(b.getData().get(), b.getSize());
        }
        writer.closeFile(os);
    }

    void SymbolsWriter::write(std::string path) {
        // output symbols to file
        // TODO also dump a mapping of code to addresses
        auto sos = writer.openFile(path);

        for (auto env : interpreter.getLabelTable()) {
            outputSymbolsEnvironment(writer, sos, env);
        }
        outputSymbolsEnvironment(writer, sos, interpreter.getGlobals());
        writer.closeFile(sos);
    }


    void SymbolsWriter::outputSymbolsEnvironment(FileWriter &writer,
            std::shared_ptr<std::ostream> os, std::shared_ptr<Environment> env) {
        // dump strings, numbers and floats only
        // in format <name> = <value>
        std::ostream &stream = *(os.get());

        auto values = env->getValues();
        for (auto it = values.begin(); it != values.end(); it++) {
            auto obj = it->second;
            std::string name = it->first;

            auto parent = env;
            while (parent.get()) {
                if (parent->getName() != "") {
                    name = parent->getName() + delim + name;
                }
                parent = parent->getParent();
            }

            switch (obj->getType()) {
                case STRING_O:
                    stream << name << " = " << obj->toString() << std::endl;
                    break;
                case NUMBER_O:
                    stream << name << " = " << hexPrefix << std::hex << obj->toNumber() << std::endl;
                    break;
                case REAL_O:
                    stream << name << " = " << obj->toReal() << std::endl;
                    break;
                default:
                    // skip
                    break;
            }
        }
    }
}
