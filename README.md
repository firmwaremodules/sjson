## SJSON: The Streaming JSON Parser

This callback-based parser parses JSON from a stream of bytes, eliminating the need to read an entire JSON object
into a pre-allocated buffer before parsing.  This is the most resource-efficient way to handle JSON.

It is designed to run on MCUs in embedded systems and the parser handles both single and multi-line comment styles embedded in the JSON!

Potential uses include parsing long or unknown length JSON response structures
from HTTP queries and handling JSON configuration files.

In a nutshell, this parser will handle arbitrary length JSON with a fixed and minimal RAM buffer requirement.


### Memory Usage

A small buffer is needed to handle the largest token size (key or value) that may be present in the stream.  Tokens that
exceed this buffer will halt parsing with an error. 

The parser maintains state in a small context structure.  Both the structure and the token buffer can be allocated 
on-demand on the stack in many cases.

### Setup

The user creates an array of `sjson_cb_t` structures, each defining a key token to match against and a callback function to
invoke with the value associated with the matched token.  This array **must contain a null-terminating entry**.

The array is passed to `sjson_init()`.

Example:

```
const sjson_cb_t my_sjson_callbacks[] = 
    { "key1", my_key1_value_handler }, /* match key value "key1" and invoke handler my_key1_value_handler() for it */
    // more handlers,
    { 0 } // null-terminator
};
```

The callback array is bound at run time so that the parser may be reconfigured to handle a variety of input streams.

The parser context structure must be initialized before use:

```
    sjson_ctx_t ctx;
    char my_token_buf[64]; /* This is the only memory required to parse arbitrary length JSON */

    sjson_init(&ctx, my_token_buf, sizeof(my_token_buf), my_sjson_callbacks);
```

## Parsing

The user invokes the `sjson_parse(sjson_ctx_t* ctx, const char* buf, int len)` function for arbitrary chunks of the JSON input stream.
This parser can be called for each byte of the input if that's what you want to do.  But more commonly, a buffer is allocated
to handle chunks of some input stream, like http response callback or a file read function.  These chunks are then forwarded to this parser.

As the parser finds keys, it will search the callback array and attempt to match them.  If a match is found, the subsequent value token, 
if found, triggers the associated callback.  The user can then do application-specific processing of that value.

The user callback is passed a pointer to the user's token buffer filled in with the parsed value string, the length of the string,
the depth within the JSON heirarchy that the key was found at, and the type of the parsed value.

This parser handles JSON primitives and strings, including escaped characters and limited unicode. 
All values, whether string or primitive, are parsed into null-terminated strings in the user token buffer.

For example, a JSON key: value pair primitive defined like this:  `"ram" : 8192`  is parsed as "8192".  The user can then convert to an integer
in their "ram" handler with atoi(), for example.

This parser also handles arrays of values by invoking the handler for each value in the array. 

This parser is inspired by the excellent [JSMN](https://github.com/zserge/jsmn) JSON parser by Serge Zaitsev.
Unlike JSMN, SJSON does not require a pre-allocated token stack nor input buffer to host the full JSON before parsing.

### Example

Example: reading from a file.

A JSON format file can be read in arbitrary block sizes and supplied to the streaming JSON parser via `sjson_parse()`.
Each invocation of `sjson_parse()` may yield multiple callback invocations depending on the number of tokens found in the stream.

```
sjson_ctx_t ctx;
char my_token_buf[64]; /* handle tokens up to 63 characters in length */

sjson_init(&ctx, my_token_buf, sizeof(my_token_buf), my_sjson_callbacks);

char file_buf[32]; /* read file in chunks of 32 bytes or less */
FILE* fid = fopen("config.json", "r");
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

```

### Limitations

* SJSON can parse objects and arrays up to 31 deep.  Beyond that the parser depth level will get out of sync and will encounter unexpected input.

### Tests

There is a reference json file `tests/json-test-mixed-small.json` demonstrating the capabilities of this parser along with a test runner `json-tests.c` demonstrating how to use the SJSON parser.




