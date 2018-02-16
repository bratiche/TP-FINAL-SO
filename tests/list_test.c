#include <check.h>
#include <stdlib.h>
#include <list.h>

START_TEST(test_list_add)
    List list = list_new();
    list_add(list, NULL);

    ck_assert_uint_eq(list_size(list), 0);
    list_add(list, "1");
    ck_assert_uint_eq(list_size(list), 1);
    list_add(list, "2");
    ck_assert_uint_eq(list_size(list), 2);
    list_add(list, "3");
    ck_assert_uint_eq(list_size(list), 3);

    list_destroy(list);
END_TEST

START_TEST(test_list_get)
    List list = list_new();
    list_add(list, "1");
    list_add(list, "2");
    list_add(list, "3");

    ck_assert_ptr_eq(list_get(list, 0), "1");
    ck_assert_ptr_eq(list_get(list, 1), "2");
    ck_assert_ptr_eq(list_get(list, 2), "3");

    list_destroy(list);
END_TEST

START_TEST(test_list_iterate)
    List list = list_new();
    list_add(list, "1");
    list_add(list, "2");
    list_add(list, "3");

    ck_assert_ptr_eq(list_get_next(list), "1");
    ck_assert_ptr_eq(list_get_next(list), "2");
    ck_assert_ptr_eq(list_get_next(list), "3");
    ck_assert_ptr_eq(list_get_next(list), NULL);
    ck_assert_ptr_eq(list_get_next(list), "1");
    ck_assert_ptr_eq(list_get_next(list), "2");
    ck_assert_ptr_eq(list_get_next(list), "3");
    ck_assert_ptr_eq(list_get_next(list), NULL);

    list_destroy(list);
END_TEST


Suite * suite(void) {
    Suite *s   = suite_create("list");
    TCase *tc  = tcase_create("list");

    tcase_add_test(tc, test_list_add);
    tcase_add_test(tc, test_list_get);
    tcase_add_test(tc, test_list_iterate);
    suite_add_tcase(s, tc);

    return s;
}

int main(void) {
    int number_failed;
    SRunner *sr  = srunner_create(suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
