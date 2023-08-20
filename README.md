# capuchin-interp

Capuchin is a fully functional interpreter for the Monkey programming language, written entirely in C. The implementation is a based on Thorsten Ball's amazing book ['Writing An Interpreter In Go'](https://interpreterbook.com/).
 
### How does  it work?
A lexer (lexical analyzer) processes the input character stream and emits tokens. The tokens are then fed into a recursive descent parser (PRATT parsing technique) which produces an AST (Abstract Syntax Tree). Evaluation is handled via tree walking, the AST is directly traversed in order to evaluate statements/expression. Objects allocated during the evaluation step are freed using a basic mark and sweep garbage collection mechanism. 


## Building
A `Makefile` is provided in the root directory of the repo. It relies on gcc for compiling and has only been tested on Ubuntu so your milage may vary if you intend to build for another target. 

The following make commands are provided: 
- `make clean` 
- `make test` - run test cases and produce report 
- `make repl` - build the REPL


## Running 
In interactive mode aka. REPL (Read Evaluate Print Loop), the user can enter a series of statements which are evaluated and the results are printed to stdout: 
```bash
ctin@ctin-VirtualBox:~/Desktop/capuchin-inter$ ./capuchin
>> let greet = fn(name) {puts("Hello " + name + "!"); }
>> greet("Constantin");
Hello Constantin!
>> let person = {"name": "john", "age":23}
>> person
{age:23, name:john}
>> person["name"]
john
>> if (person["age"] > 18) { puts("Can legaly drink!");} else {puts("Not old enough!");}
Can legaly drink!
>> 93 * 12 - 10 / 2
1111
>> quit
```

Capuchin interpreter can execute Monkey scripts directly. Consider the following example script(available in `demos/map.mkey`):
```js
let map = fn(arr, f) { 
    let iter = fn(arr, accumulated) { 
        if (len(arr) == 0) { 
            accumulated 
        } else { 
            iter(rest(arr), push(accumulated, f(first(arr)))); 
        } 
    }; 

    iter(arr, []); 
};
let a = [0, 2, 3, 4];
let double = fn(x) { x + 1 };

map(a, double);
```
Any Monkey script can be executed using command `./capuchin <path to script>`. For example: 
```bash 
ctin@ctin-VirtualBox:~/Desktop/c-inter$ ./capuchin ./demos/map.mkey 
[0, 4, 6, 8]
```
Note: In this mode results of intermediate statements are silenced (not printed to stdout like in interactive mode). In order to output information to the console explicit calls to `puts(<object>)` or `printf(<format>, ...)` must be placed within the script. 

