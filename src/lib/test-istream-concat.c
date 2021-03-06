/* Copyright (c) 2009-2014 Dovecot authors, see the included COPYING file */

#include "test-lib.h"
#include "istream-private.h"
#include "istream-concat.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define TEST_MAX_ISTREAM_COUNT 10
#define TEST_MAX_ISTREAM_SIZE 1024
#define TEST_MAX_BUFFER_SIZE 128

static void test_istream_concat_one(unsigned int buffer_size)
{
	static const char *input_string = "xyz";
#define STREAM_COUNT 5
#define STREAM_BYTES 3
	struct istream *streams[STREAM_COUNT+1];
	struct istream *input;
	const unsigned char *data;
	size_t size;
	unsigned int i, j;

	for (i = 0; i < STREAM_COUNT; i++) {
		streams[i] = test_istream_create(input_string);
		test_istream_set_allow_eof(streams[i], TRUE);
		test_istream_set_size(streams[i], 0);
	}
	streams[i] = NULL;

	input = i_stream_create_concat(streams);
	for (i = 0; i/STREAM_BYTES < STREAM_COUNT; i++) {
		test_istream_set_size(streams[i/STREAM_BYTES], (i%STREAM_BYTES) + 1);
		test_assert(i_stream_read(input) == 1);
		if (i < buffer_size) {
			data = i_stream_get_data(input, &size);
			test_assert(size == i+1);
		} else {
			i_stream_skip(input, 1);
			data = i_stream_get_data(input, &size);
			test_assert(size == buffer_size);
		}
		for (j = 0; j < size; j++) {
			test_assert((char)data[j] == input_string[(input->v_offset + j) % STREAM_BYTES]);
		}
	}
	i_stream_unref(&input);
}

static void test_istream_concat_random(void)
{
	struct istream **streams, *input, *input1, *input2;
	const unsigned char *data;
	unsigned char *w_data;
	size_t size = 0;
	unsigned int i, j, offset, stream_count, data_len;

	srand(3);
	stream_count = (rand() % TEST_MAX_ISTREAM_COUNT) + 2;
	streams = t_new(struct istream *, stream_count + 1);
	for (i = 0, offset = 0; i < stream_count; i++) {
		data_len = rand() % TEST_MAX_ISTREAM_SIZE + 1;
		w_data = t_malloc(data_len);
		for (j = 0; j < data_len; j++)
			w_data[j] = offset++;
		streams[i] = test_istream_create_data(w_data, data_len);
		test_istream_set_allow_eof(streams[i], TRUE);
	}
	streams[i] = NULL;
	i_assert(offset > 0);

	input = i_stream_create_concat(streams);
	i_stream_set_max_buffer_size(input, TEST_MAX_BUFFER_SIZE);
	input1 = i_stream_create_limit(input, (uoff_t)-1);
	input2 = i_stream_create_limit(input, (uoff_t)-1);
	i_stream_unref(&input);

	for (i = 0; i < 1000; i++) {
		input = i % 2 == 0 ? input1 : input2;
		if (rand() % 3 == 0) {
			i_stream_seek(input, rand() % offset);
		} else {
			ssize_t ret = i_stream_read(input);
			size = i_stream_get_data_size(input);
			if (ret == -2) {
				test_assert(size >= TEST_MAX_BUFFER_SIZE);
			} else if (input->v_offset + size != offset) {
				test_assert(ret > 0);
				test_assert(input->v_offset + ret <= offset);
				i_stream_skip(input, rand() % ret);

				data = i_stream_get_data(input, &size);
				for (j = 0; j < size; j++) {
					test_assert(data[j] == (input->v_offset + j) % 256);
				}
			}
		}
	}
	for (i = 0; i < stream_count; i++)
		i_stream_unref(&streams[i]);
	i_stream_unref(&input1);
	i_stream_unref(&input2);
}

void test_istream_concat(void)
{
	unsigned int i;

	test_begin("istream concat");
	for (i = 1; i < STREAM_BYTES*STREAM_COUNT; i++) {
		test_istream_concat_one(i);
	}
	test_end();

	test_begin("istream concat random");
	for (i = 0; i < 100; i++) T_BEGIN {
		test_istream_concat_random();
	} T_END;
	test_end();
}
