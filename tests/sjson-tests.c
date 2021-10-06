/*
 * Copyright (c) 2021 Firmware Modules Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this softwareand associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright noticeand this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


#define DEBUG 1 
#include "..\sjson.h"

#if DEBUG
#define PRINTF(...)  printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* User token buffer to test with */
#define SJSON_TOKEN_BUF_SIZE        64

/* Test various chunk sizes */
#define TEST_CHUNK_SIZE_START       1
#define TEST_CHUNK_SIZE_END         5

/* Setup the SJSON parser for the included test file:
 *   json-test-mixed-small.json
 */

static int image_handler(const char* buf, uint16_t len, sjson_type_t type, uint8_t depth)
{
    printf("Image @%d!\n", depth);
    /* Should NOT get called because its value is an object */
    return SJSON_STATUS_ERROR;
}

static int generic_handler(const char* buf, uint16_t len, sjson_type_t type, uint8_t depth)
{
    printf("[HANDLER] [%s] len:%d depth:%d type:%d\n", buf, len, depth, type);
    return SJSON_STATUS_OK;
}

const sjson_cb_t sjson_callbacks[] = {
    { "Image", image_handler},
    { "Width", generic_handler},
    { "Height", generic_handler},
    { "Title", generic_handler},
    { "Thumbnail", generic_handler},
    { "Url", generic_handler},
    { "Animated", generic_handler},
    { "IDs", generic_handler},
    { "Escaped", generic_handler},
    { "Unicode", generic_handler},
    { "IDstrings", generic_handler},
    { 0 }
};

void test_utility(void)
{
    const char* buf1 = "6546cabaf079a6221d589f438654048e3a3b70a213739c6938ae8c358e8b55c8";
    const char* buf2 = "6546cabaf079a6221d589f438654048e3a3b70a213739c6938ae8c358e8b55c"; /* not divisible by 2 */
    const char* buf3 = "6546cabaf079a6221d589f438654048e3a3b70a213739c6938ae8c358e8b55c8d"; /* too large */
    const uint8_t expect1[] = {
        0x65,0x46,0xca,0xba,0xf0,0x79,0xa6,0x22,0x1d,
        0x58,0x9f,0x43,0x86,0x54,0x04,0x8e,0x3a,0x3b,
        0x70,0xa2,0x13,0x73,0x9c,0x69,0x38,0xae,0x8c,
        0x35,0x8e,0x8b,0x55,0xc8 
    };

    printf("Test Utility functions\n");

    { /* Compare OK SUCCESS */
        int res = 0;
        uint8_t* resp = (uint8_t*)malloc(sizeof(expect1));

        printf("Test sjson_pack_hexstr2bin(buf1)  %d\n", (int)sizeof(expect1));
        res = sjson_pack_hexstr2bin(buf1, resp, sizeof(expect1));

        if (memcmp(expect1, resp, sizeof(expect1)) == 0) {
            printf("Compare OK ");
        }
        else {
            printf("Compare FAIL ");
        }

        if (res == 0) {
            printf("SUCCESS\n");
        }
        else {
            printf("ERROR: %s\n", sjson_status_to_str(res));
        }

        free(resp);
    }

    { /* Compare FAIL ERROR: UNEXPECTED_INPUT */
        int res = 0;
        uint8_t* resp = (uint8_t*)malloc(sizeof(expect1));

        printf("Test sjson_pack_hexstr2bin(buf2)  %d\n", (int)sizeof(expect1));
        res = sjson_pack_hexstr2bin(buf2, resp, sizeof(expect1));

        if (memcmp(expect1, resp, sizeof(expect1)) == 0) {
            printf("Compare OK ");
        }
        else {
            printf("Compare FAIL ");
        }

        if (res == 0) {
            printf("SUCCESS\n");
        }
        else {
            printf("ERROR: %s\n", sjson_status_to_str(res));
        }

        free(resp);
    }

    { /* Compare OK ERROR: OVERFLOW */
        int res = 0;
        uint8_t* resp = (uint8_t*)malloc(sizeof(expect1));

        printf("Test sjson_pack_hexstr2bin(buf3)  %d\n", (int)sizeof(expect1));
        res = sjson_pack_hexstr2bin(buf3, resp, sizeof(expect1));

        if (memcmp(expect1, resp, sizeof(expect1)) == 0) {
            printf("Compare OK ");
        }
        else {
            printf("Compare FAIL ");
        }

        if (res == 0) {
            printf("SUCCESS\n");
        }
        else {
            printf("ERROR: %s\n", sjson_status_to_str(res));
        }

        free(resp);
    }

}

int main(int argc, char **argv) 
{
    int status = 0;

    /* Test utility */
    test_utility();


    if (argc < 2) {
        printf("Usage: sjson-tests [json-file]\n");
        return 1;
    }


    /*  Open JSON file */
    FILE *json = fopen(argv[1], "rb");

    if (!json) {
        printf("Could not open '%s'\n", argv[1]);
        return 1;
    }
   
    sjson_ctx_t ctx;
    int res = sjson_init(
        &ctx,
        malloc(SJSON_TOKEN_BUF_SIZE),
        SJSON_TOKEN_BUF_SIZE, 
        sjson_callbacks);

    if (res != SJSON_STATUS_OK) {
        printf("sjson_init() error %d\n", res);
        return 1;
    }
    

    /* Feed in JSON file... */

    fseek(json, 0L, SEEK_END);
    const size_t json_size = ftell(json);

    for (int chunkSize = TEST_CHUNK_SIZE_START; chunkSize <= TEST_CHUNK_SIZE_END; chunkSize++)
    {
        uint32_t remainingSize = json_size;

        printf("Feeding in json file of size %d in %d byte chunks\n", (int)json_size, (int)chunkSize);
        fseek(json, 0L, SEEK_SET);
        char* buf = malloc(chunkSize);
        int i = 0;
        for (; i < json_size; i += chunkSize, remainingSize -= chunkSize)
        {
            if (remainingSize < chunkSize)
            {
                fread(buf, 1, remainingSize, json);

                status = sjson_parse(&ctx, buf, remainingSize);
            }
            else
            {
                fread(buf, 1, chunkSize, json);

                status = sjson_parse(&ctx, buf, chunkSize);
            }

            if (status < 0) {
                break;
            }
        }

        printf("%s ", (i >= json_size ? "Completed" : "Incomplete"));
        if (status == SJSON_STATUS_OK) {
            printf("without error: %s\n", sjson_status_to_str(status));
        }
        else {
            printf("with error: %s\n", sjson_status_to_str(status));
        }

        printf("-- chars:%d keys:%d strings:%d prims:%d calls:%d\n",
            ctx.stats.char_count,
            ctx.stats.num_keys,
            ctx.stats.num_strings,
            ctx.stats.num_primitives,
            ctx.stats.num_callbacks);

        free(buf);
        sjson_reset(&ctx);

        if (status < 0) {
            return status;
        }
    }


    /* Another way to read and parse the JSON file. */
    {
        sjson_ctx_t ctx;
        char my_token_buf[64]; /* handle tokens up to 63 characters in length */

        sjson_init(&ctx, my_token_buf, sizeof(my_token_buf), sjson_callbacks);

        char file_buf[32]; /* read file in chunks of 32 bytes or less */
        FILE* fid = fopen(argv[1], "r");
        int len = 0;
        int status = 0;
        do {
            len = fread(file_buf, 1, sizeof(file_buf), fid);
            status = sjson_parse(&ctx, file_buf, len);
        } while (len > 0 && status >= 0);

        printf("Done: %s chars:%d keys:%d strings:%d prims:%d calls:%d\n",
            sjson_status_to_str(status),
            ctx.stats.char_count,
            ctx.stats.num_keys,
            ctx.stats.num_strings,
            ctx.stats.num_primitives,
            ctx.stats.num_callbacks);
    }


    return 0;
}

