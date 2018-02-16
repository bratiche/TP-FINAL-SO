#include <check.h>
#include <stdlib.h>
#include <database/request_parser.h>

extern bool debug;

START_TEST(test_request_add_client)
    RequestParser parser;

    request_parser_init(&parser);
    request_state state = request_parser_consume(&parser, "0\nclient_name\n.\n");

    ck_assert_uint_eq(state, request_done);
    ck_assert_uint_eq(parser.request->type, ADD_CLIENT);
    ck_assert_uint_eq(parser.request->argc, 1);
    ck_assert_str_eq(parser.request->args[0], "client_name");

    request_parser_destroy(&parser);
END_TEST

START_TEST(test_request_add_booking)
    RequestParser parser;

    request_parser_init(&parser);
    request_state state = request_parser_consume(&parser, "6\nclient_name\nmovie_name\n4\n4\n2\n.\n");

    ck_assert_uint_eq(state, request_done);
    ck_assert_uint_eq(parser.request->type, ADD_BOOKING);
    ck_assert_uint_eq(parser.request->argc, 5);
    ck_assert_str_eq(parser.request->args[0], "client_name");
    ck_assert_str_eq(parser.request->args[1], "movie_name");
    ck_assert_str_eq(parser.request->args[2], "4");
    ck_assert_str_eq(parser.request->args[3], "4");
    ck_assert_str_eq(parser.request->args[4], "2");

    request_parser_destroy(&parser);
END_TEST

START_TEST(test_request_invalid_cmd)
    RequestParser parser;

    request_parser_init(&parser);
    request_state state = request_parser_consume(&parser, "-1\n.\n");

    ck_assert_uint_eq(state, request_error_invalid_cmd);

    request_parser_destroy(&parser);
END_TEST

START_TEST(test_request_arg_too_long)
    RequestParser parser;

    request_parser_init(&parser);
    request_state state = request_parser_consume(&parser, "0\n");

    for (int i = 0; i < ARG_SIZE * 2; i++) {
        state = request_parser_consume(&parser, "-");
        if (state > request_done) {
            break;
        }
    }

    ck_assert_uint_eq(state, request_error_argument_too_long);

    request_parser_destroy(&parser);
END_TEST

START_TEST(test_request_too_many_args)
    RequestParser parser;

    request_parser_init(&parser);
    request_state state = request_parser_consume(&parser, "0\n");

    for (int i = 0; i < MAX_ARGS * 2; i++) {
        state = request_parser_consume(&parser, "-\n");
        if (state > request_done) {
            break;
        }
    }

    ck_assert_uint_eq(state, request_error_too_many_arguments);

    request_parser_destroy(&parser);
END_TEST


Suite * suite() {
    Suite *s = suite_create("request");
    TCase *tc = tcase_create("request");

    tcase_add_test(tc, test_request_add_client);
    tcase_add_test(tc, test_request_add_booking);
    tcase_add_test(tc, test_request_invalid_cmd);
    tcase_add_test(tc, test_request_arg_too_long);
    tcase_add_test(tc, test_request_too_many_args);

    suite_add_tcase(s, tc);

    return s;
}

int main(int argc, char * argv[]) {
    debug = true;
    int number_failed;
    SRunner *sr = srunner_create(suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
