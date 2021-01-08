#include "enviorment.h"

#include "macros.h"

using namespace lasm;

void test_enviorment(void **state) {
    Enviorment env;
    LasmObject obj(NUMBER_O, lasmNumber(123));
    env.define("test", obj);

    std::shared_ptr<Token> token = std::make_shared<Token>(Token(IDENTIFIER, "test",
                LasmObject(NIL_O, nullptr), -1, ""));
    auto foundObj = env.get(token);
    assert_int_equal(foundObj->getType(), NUMBER_O);
    assert_int_equal(foundObj->toNumber(), 123);

    // exception if not found
    std::shared_ptr<Token> notFound = std::make_shared<Token>(Token(IDENTIFIER, "test2",
                LasmObject(NIL_O, nullptr), -1, ""));
    assert_throws(LasmUndefinedReference, {
        env.get(notFound);
    });
}
