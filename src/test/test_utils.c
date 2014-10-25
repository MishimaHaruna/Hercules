// Copyright (c) Hercules Dev Team, licensed under GNU GPL.
// See the LICENSE file

#include "testsupport.h"

#include "../common/utils.h"

static char temporary_buffer[1024];
char *mock_SERVER_NAME = NULL;

void mock_ShowError(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vsnprintf(temporary_buffer, sizeof(temporary_buffer), format, args);
	check_expected(temporary_buffer);
	va_end(args);
}
void mock_ShowWarning(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vsnprintf(temporary_buffer, sizeof(temporary_buffer), format, args);
	check_expected(temporary_buffer);
	va_end(args);
}
void mock_ShowDebug(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vsnprintf(temporary_buffer, sizeof(temporary_buffer), format, args);
	check_expected(temporary_buffer);
	va_end(args);
}

static void test_WriteDump(void **state) {
	(void)state; // unused
	// TODO
	/* void WriteDump(FILE* fp, const void* buffer, size_t length); */
}

static void test_ShowDump(void **state) {
	(void)state; // unused
	char buf[0x10] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };
	expect_string(mock_ShowDebug, temporary_buffer, "--- 00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F   0123456789ABCDEF\n");
	expect_string(mock_ShowDebug, temporary_buffer, "000 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F   ................\n");
	ShowDump(buf, sizeof(buf));
}

static void test_findfile(void **state) {
	(void)state; // unused
	// TODO
	/* void findfile(const char *p, const char *pat, void (func)(const char*)); */
}
static void test_exists(void **state) {
	(void)state; // unused
	assert_true(exists(mock_SERVER_NAME));
	assert_false(exists("This File Does Not Exist :)"));
}

/**
 * Tests the cap_value macro
 */
static void test_cap_value(void **state) {
	(void)state; // unused
	assert_int_equal(cap_value(1, 0, 10), 1);
	assert_int_equal(cap_value(0, 1, 10), 1);
	assert_int_equal(cap_value(20, 1, 10), 10);
	assert_int_equal(cap_value(-1, 1, 10), 1);
	assert_int_equal(cap_value(0, -1, 1), 0);
	assert_int_equal(cap_value(-10, -1, 1), -1);
	assert_int_equal(cap_value(10, -10, 0), 0);
}

static void test_get_percentage(void **state) {
	(void)state; // unused
	assert_int_equal(get_percentage(1, 100), 1);
	assert_int_equal(get_percentage(50, 100), 50);
	assert_int_equal(get_percentage(100, 100), 100);
	assert_int_equal(get_percentage(1, 1000), 0);
	assert_int_equal(get_percentage(19, 1000), 1);
}

static void test_timestamp2string(void **state) {
	(void)state; // unused
	char buf[256] = { 0 };
	int y, m, d, H, M, S;
	assert_true(timestamp2string(buf, 256, time(NULL), "%Y-%m-%d %H:%M:%S"));
	assert_int_equal(sscanf(buf, "%d-%d-%d %d:%d:%d", &y, &m, &d, &H, &M, &S), 6);
}

/**
 * Tests the Get/Make Byte/Word/DWord functions
 */
static void test_byte_word_dword_access(void **state) {
	(void)state; // unused
	assert_true(GetByte(0x12345678, 0) == 0x78);
	assert_true(GetByte(0x12345678, 1) == 0x56);
	assert_true(GetByte(0x12345678, 2) == 0x34);
	assert_true(GetByte(0x12345678, 3) == 0x12);
	assert_true(GetWord(0x12345678, 0) == 0x5678);
	assert_true(GetWord(0x12345678, 1) == 0x1234);
	assert_true(MakeWord(0x78, 0x56) == 0x5678);
	assert_true(MakeDWord(0x5678, 0x1234) == 0x12345678);
}

static void test_big_endian_compat(void **state) {
	(void)state; // unused
	union {
		struct {
			int8 b0, b1, b2, b3;
		} i8;
		struct {
			int16 w0, w1;
		} i16;
		struct {
			int32 dw0;
		} i32;
		unsigned char buf[32];
	} data;
	data.i8.b0 = 0x78;
	data.i8.b1 = 0x56;
	data.i8.b2 = 0x34;
	data.i8.b3 = 0x12;
	assert_true(MakeShortLE(0x5678) == data.i16.w0);
	assert_true(MakeLongLE(0x12345678) == data.i32.dw0);
	assert_true(GetUShort(data.buf) == 0x5678);
	assert_true(GetULong(data.buf) == 0x12345678);
	assert_true(GetLong(data.buf) == 0x12345678);
	data.i8.b3 = 0x3e;
	data.i8.b2 = 0x20;
	data.i8.b1 = data.i8.b0 = 0x00;
	assert_true(GetFloat(data.buf) < 0.156251 && GetFloat(data.buf) > 0.156249);
}

// TODO
/* size_t hread(void * ptr, size_t size, size_t count, FILE * stream); */
/* size_t hwrite(const void * ptr, size_t size, size_t count, FILE * stream); */

/* [Ind/Hercules] Caching */
/* struct HCache_interface { */
/* 	void (*init) (void); */
/* 	/1* *1/ */
/* 	bool (*check) (const char *file); */
/* 	FILE *(*open) (const char *file, const char *opt); */
/* 	/1* *1/ */
/* 	time_t recompile_time; */
/* 	bool enabled; */
/* }; */

/* struct HCache_interface *HCache; */

/* void HCache_defaults(void); */

int main(int argc, char **argv) {
	mock_SERVER_NAME = argv[0];
	UnitTest tests[] = {
		unit_test(test_WriteDump),
		unit_test(test_ShowDump),
		unit_test(test_findfile),
		unit_test(test_exists),
		unit_test(test_cap_value),
		unit_test(test_get_percentage),
		unit_test(test_timestamp2string),
		unit_test(test_byte_word_dword_access),
		unit_test(test_big_endian_compat),
	};
	return run_tests(tests);
}
