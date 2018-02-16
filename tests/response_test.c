#include <check.h>
#include <stdlib.h>
#include <protocol.h>
#include <client/response_parser.h>

extern bool debug;

START_TEST(test_response_ok)
    ResponseParser parser;
    Response * response = new_response();
    response_parser_init(&parser, response);
    response_parser_consume(&parser, "0\n.\n");

    ck_assert_uint_eq(parser.state, response_done);
    ck_assert_uint_eq(response->status, RESPONSE_OK);
    ck_assert_uint_eq(response->argc, 0);

    destroy_response(response);
    response_parser_destroy(&parser);
END_TEST

START_TEST(test_response_ok_with_args)
    ResponseParser parser;
    Response * response = new_response();
    response_parser_init(&parser, response);
    response_parser_consume(&parser, "0\narg1\narg2\narg3\narg4\narg5\narg6\narg7\narg8\narg9\narg10\n.\n");

    ck_assert_uint_eq(parser.state, response_done);
    ck_assert_uint_eq(response->status, RESPONSE_OK);
    ck_assert_uint_eq(response->argc, 10);
    ck_assert_str_eq(response->args[0], "arg1");
    ck_assert_str_eq(response->args[1], "arg2");
    ck_assert_str_eq(response->args[2], "arg3");
    ck_assert_str_eq(response->args[3], "arg4");
    ck_assert_str_eq(response->args[4], "arg5");
    ck_assert_str_eq(response->args[5], "arg6");
    ck_assert_str_eq(response->args[6], "arg7");
    ck_assert_str_eq(response->args[7], "arg8");
    ck_assert_str_eq(response->args[8], "arg9");
    ck_assert_str_eq(response->args[9], "arg10");

    destroy_response(response);
    response_parser_destroy(&parser);
END_TEST

START_TEST(test_response_err)
    ResponseParser parser;
    Response * response = new_response();
    response_parser_init(&parser, response);
    response_parser_consume(&parser, "1\n.\n");

    ck_assert_uint_eq(parser.state, response_done);
    ck_assert_uint_eq(response->status, RESPONSE_ERR);
    ck_assert_uint_eq(response->argc, 0);

    destroy_response(response);
    response_parser_destroy(&parser);
END_TEST

START_TEST(test_response_already_exists)
    ResponseParser parser;
    Response * response = new_response();
    response_parser_init(&parser, response);
    response_parser_consume(&parser, "2\n.\n");

    ck_assert_uint_eq(parser.state, response_done);
    ck_assert_uint_eq(response->status, ALREADY_EXIST);
    ck_assert_uint_eq(response->argc, 0);

    destroy_response(response);
    response_parser_destroy(&parser);
END_TEST

START_TEST(test_response_extract_seats)
    ResponseParser parser;
    Response * response = new_response();
    response_parser_init(&parser, response);
    response_parser_consume(&parser, "0\n");
    for (int i = 0; i < SEATS; i++) {
        response_parser_consume(&parser, "1\n");
        ck_assert_uint_eq(parser.state, response_args);
    }
    response_parser_consume(&parser, ".\n");

    ck_assert_uint_eq(parser.state, response_done);
    ck_assert_uint_eq(response->status, RESPONSE_OK);
    ck_assert_uint_eq(response->argc, SEATS);

    int seats[SEATS];
    response_extract_seats(response, seats);
    for (int i = 0; i < SEATS; i++) {
        ck_assert_str_eq(response->args[i], "1");
        ck_assert_uint_eq(seats[i], EMPTY_SEAT);
    }

    destroy_response(response);
    response_parser_destroy(&parser);
END_TEST

START_TEST(test_response_extract_movies)
    ResponseParser parser;
    Response * response = new_response();
    response_parser_init(&parser, response);
    response_parser_consume(&parser, "0\n");
    response_parser_consume(&parser, "movie 1\n");
    response_parser_consume(&parser, "movie 2\n");
    response_parser_consume(&parser, "movie 3\n");
    response_parser_consume(&parser, ".\n");

    ck_assert_uint_eq(parser.state, response_done);
    ck_assert_uint_eq(response->status, RESPONSE_OK);
    ck_assert_uint_eq(response->argc, 3);

    List movies = response_extract_movies(response);

    ck_assert_uint_eq(list_size(movies), 3);
    char * movie1 = list_get_next(movies);
    ck_assert_str_eq(movie1, "movie 1");
    char * movie2 = list_get_next(movies);
    ck_assert_str_eq(movie2, "movie 2");
    char * movie3 = list_get_next(movies);
    ck_assert_str_eq(movie3, "movie 3");

    ck_assert_ptr_eq(list_get_next(movies), NULL);

    list_destroy(movies);
    destroy_response(response);
    response_parser_destroy(&parser);
END_TEST

START_TEST(test_response_extract_showcases)
    ResponseParser parser;
    Response * response = new_response();
    response_parser_init(&parser, response);
    response_parser_consume(&parser, "0\n");
    response_parser_consume(&parser, "movie 1\n2\n3\n");
    response_parser_consume(&parser, "movie 2\n3\n4\n");
    response_parser_consume(&parser, ".\n");

    ck_assert_uint_eq(parser.state, response_done);
    ck_assert_uint_eq(response->status, RESPONSE_OK);
    ck_assert_uint_eq(response->argc, 6);

    List showcases = response_extract_showcases(response);

    Showcase * showcase1 = list_get_next(showcases);
    ck_assert_str_eq(showcase1->movie_name, "movie 1");
    ck_assert_uint_eq(showcase1->day, TUE);
    ck_assert_uint_eq(showcase1->room, 3);
    destroy_showcase(showcase1);

    Showcase * showcase2 = list_get_next(showcases);
    ck_assert_str_eq(showcase2->movie_name, "movie 2");
    ck_assert_uint_eq(showcase2->day, WED);
    ck_assert_uint_eq(showcase2->room, 4);
    destroy_showcase(showcase2);

    ck_assert_ptr_eq(list_get_next(showcases), NULL);

    list_destroy(showcases);
    destroy_response(response);
    response_parser_destroy(&parser);
END_TEST

START_TEST(test_response_extract_tickets)
    ResponseParser parser;
    Response * response = new_response();
    response_parser_init(&parser, response);
    response_parser_consume(&parser, "0\n");
    response_parser_consume(&parser, "movie 1\n2\n3\n4\n");
    response_parser_consume(&parser, "movie 2\n3\n4\n5\n");
    response_parser_consume(&parser, ".\n");

    ck_assert_uint_eq(parser.state, response_done);
    ck_assert_uint_eq(response->status, RESPONSE_OK);
    ck_assert_uint_eq(response->argc, 8);

    List tickets = response_extract_tickets(response);

    Ticket * ticket1 = list_get_next(tickets);
    ck_assert_str_eq(ticket1->showcase.movie_name, "movie 1");
    ck_assert_uint_eq(ticket1->showcase.day, TUE);
    ck_assert_uint_eq(ticket1->showcase.room, 3);
    ck_assert_uint_eq(ticket1->seat, 4);
    destroy_ticket(ticket1);

    Ticket * ticket2 = list_get_next(tickets);
    ck_assert_str_eq(ticket2->showcase.movie_name, "movie 2");
    ck_assert_uint_eq(ticket2->showcase.day, WED);
    ck_assert_uint_eq(ticket2->showcase.room, 4);
    ck_assert_uint_eq(ticket2->seat, 5);
    destroy_ticket(ticket2);

    ck_assert_ptr_eq(list_get_next(tickets), NULL);

    list_destroy(tickets);
    destroy_response(response);
    response_parser_destroy(&parser);
END_TEST


Suite * suite() {
    Suite *s = suite_create("response");
    TCase *tc = tcase_create("response");

    tcase_add_test(tc, test_response_ok);
    tcase_add_test(tc, test_response_ok_with_args);
    tcase_add_test(tc, test_response_err);
    tcase_add_test(tc, test_response_already_exists);
    tcase_add_test(tc, test_response_extract_seats);
    tcase_add_test(tc, test_response_extract_movies);
    tcase_add_test(tc, test_response_extract_showcases);
    tcase_add_test(tc, test_response_extract_tickets);

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
