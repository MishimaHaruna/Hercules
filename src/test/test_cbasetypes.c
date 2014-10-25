// Copyright (c) Hercules Dev Team, licensed under GNU GPL.
// See the LICENSE file

#include "testsupport.h"

#include "../common/cbasetypes.h"

/**
 * Ensures that __func__ is properly defined and works correctly.
 */
static void test_func__(void **state) {
	(void)state; // unused
	assert_string_equal(__func__, "test_func__");
}

/**
 * Ensure sanity of unsigned constant comparisons.
 */
static void test_unsigned_constant_comparisions(void **state) {
	(void)state; // unused
	assert_true(-20 < UCHAR_MAX);
	assert_true(-20 < UINT8_MAX);
	assert_true(-20 < USHRT_MAX);
	assert_true(-20 < UINT16_MAX);
	assert_true(-20 < UINT_MAX);
	assert_true(-20 < UINT32_MAX);
}

/**
 * Ensure size of the custom integral types.
 */
static void test_int_type_sizes(void **state) {
	(void)state; // unused
	assert_int_equal(sizeof(uint8), 1);
	assert_int_equal(sizeof(uint16), 2);
	assert_int_equal(sizeof(uint32), 4);
	assert_int_equal(sizeof(uint64), 8);
	assert_int_equal(sizeof(sint8), 1);
	assert_int_equal(sizeof(sint16), 2);
	assert_int_equal(sizeof(sint32), 4);
	assert_int_equal(sizeof(sint64), 8);
	assert_int_equal(sizeof(int8), 1);
	assert_int_equal(sizeof(int16), 2);
	assert_int_equal(sizeof(int32), 4);
	assert_int_equal(sizeof(int64), 8);
	assert_true(sizeof(ppint8) >= 1);
	assert_true(sizeof(ppint16) >= 2);
	assert_true(sizeof(ppint32) >= 4);
	assert_true(sizeof(ppuint8) >= 1);
	assert_true(sizeof(ppuint16) >= 2);
	assert_true(sizeof(ppuint32) >= 4);
	assert_true(sizeof(intptr) == sizeof(void*));
	assert_true(sizeof(uintptr) == sizeof(void*));
}

/**
 * Ensure boolean types sanity.
 */
static void test_bool(void **state) {
	(void)state; // unused
	assert_true(true);
	assert_false(false);
	assert_false(NULL);
}

/**
 * Tests the swap and check macros.
 */
static void test_swap_check_macros(void **state) {
	(void)state; // unused
	int a = 1, b = 2;
	int *pa = &a;
	int *pb = &b;
	swap(a, b);
	assert_int_equal(a, 2);
	assert_int_equal(b, 1);
	swap_ptr(pa, pb);
	assert_true(pa == &b);
	assert_true(pb == &a);
	assert_int_equal(max(1, 2), 2);
	assert_int_equal(min(1, 2), 1);
	assert_int_equal(max(-1, 2), 2);
	assert_int_equal(min(-1, 2), -1);
	assert_int_equal(max(-1, -2), -1);
	assert_int_equal(min(-1, -2), -2);
}

int main(void) {
	UnitTest tests[] = {
		unit_test(test_func__),
		unit_test(test_unsigned_constant_comparisions),
		unit_test(test_int_type_sizes),
		unit_test(test_bool),
		unit_test(test_swap_check_macros),
	};
	return run_tests(tests);
}
