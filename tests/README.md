## Testing

The test executable `sjson-tests.exe` can be built on a Windows host using MinGW toolchain using this [guide](https://www.eclipse.org/4diac/documentation/html/installation/minGW.html).  Installer from [here](https://sourceforge.net/projects/mingw-w64/files/).

Download installer and choose installation for x86_64 and win32 threads. Install to c:\mingw-w64.  Rename mingw32-make.exe to make.exe.
Then switch to `c:\mingw-w64\x86_64-8.1.0-win32-seh-rt_v6-rev0` and run ` mingw-w64.bat` to add `make` to the path.

Run `make` in `tests` directory to build the test runner `sjson-tests.exe` or whatever your build platform produces.

Run tests with:

* `sjson-tests <json file to test>`

The test executable is designed to print matches for the keys in the included
test file `json-test-mixed-small.json`.

Example test output:

```
Feeding in json file of size 485 in 5 byte chunks
[HANDLER] [800] depth:2 type:2
[HANDLER] [600] depth:2 type:2
[HANDLER] [View from 15th Floor] depth:2 type:1
[HANDLER] [http://www.example.com/image/481989943] depth:3 type:1
[HANDLER] [125] depth:3 type:2
[HANDLER] [100] depth:3 type:2
[HANDLER] [false] depth:2 type:2
[HANDLER] [116] depth:2 type:2
[HANDLER] [943] depth:2 type:2
[HANDLER] [234] depth:2 type:2
[HANDLER] [38793] depth:2 type:2
.HANDLER] [."./.\..
.       .] depth:2 type:1
[HANDLER] [Unicode Omega 234:] depth:2 type:1
[HANDLER] [id1] depth:2 type:1
[HANDLER] [id2] depth:2 type:1
[HANDLER] [id   3] depth:2 type:1
Completed without error: OK
-- chars:485 keys:13 strings:7 prims:9 calls:16
```


