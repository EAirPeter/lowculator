LOWCULATOR
==========

a lowb's calculator in C.

## Build

Use `cmake` to build this project.

This project is compiled with `-Wall -Wconversion -Werror -Wextra -Wformat -std=c11`.

## Usage

Use *`COMMANDLINE`* to start the program.

### *`EXECUTABLE`*

- `lowculator`

### *`COMMANDLINE`*

- *`EXECUTABLE COMMANDs`*
 - Process all commands one by one.
- *`EXECUTABLE`*
 - *`EXECUTABLE `*`-f --`

### *`COMMAND`*

Every *`COMMAND`* is equivalent to a *`DIRECTIVE`*.

- `-c `*`EXPRESSION`*
 - Equivalent to `#precision `*`EXPRESSION`*
- `-e `*`EXPRESSION`*
 - Equivalent to `#evaluate `*`EXPRESSION`*
- `-f `*`FILENAME`*
 - Equivalent to `#file `*`FILENAME`*
- `-o `*`FILENAME`*
 - Equivalent to `#output `*`FILENAME`*
- `-p (on|off)`
 - Equivalent to `#panic (on|off)`
- `-x `*`EXPRESSION`*
 - Equivalent to `#exit `*`EXPRESSION`*

### *`DIRECTIVE`*

Any *`DIRECTIVE`* begins with `#`.

If a *`DIRECTIVE`* is ill-formed, there will be an *`ERROR_MESSAGE`*.

- `#eof`
 - End of file. That means, the current input file will be closed immediately. (except `stdin`).
- `#evaluate `*`EXPRESSION`*
 - Evaluate an expression and print the result.
- `#exit `*`EXPRESSION`*
 - Exit the program with the given exitcode. (round to integer).
- `#file `*`FILENAME`*
 - Process the given file which contains *`EXPRESSIONs`* and *`DIRECTIVEs`*.
- `#precision `*`EXPRESSION`*
 - Specify the precision (round to integer, non-negative and less than `20`) of the result to print.
 - (default `8`)
- `#output `*`FILENAME`*
 - Close the current output file (except `stdout`), and open the given file for printing following results.
 - (default `stdout`)
- `#panic (on|off)`
 - Turn on/off panic mode. If turned on, any failed operation shall cause the program exited.
 - (default `off`)

### *`FILENAME`*

- A string.
 - The path to the file.
- `--`
 - `stdin` or `stdout`

### *`EXPRESSION`*

If an *`EXPRESSION`* is ill-formed or any step when evaluating it has domain error,
there will be an ```ERROR_MESSAGE``` printed to `stderr`.

If any step when evaluating an *`EXPRESSION`* has something wrong, like overflow or underflow, the result can be wrong.

- *`WS EXPRESSION`*
- `(`*`EXPRESSION`*`) `*`WS`*
- *`NUMBER`*
- *`INVOCATION`*
- *`UNARY_OPR EXPRESSION`*
- *`EXPRESSION BINARY_OPR EXPRESSION`*

#### *`NUMBER`*

- *`BINARY_FLOATING`*
- *`OCTAL_FLOATING`*
- *`DECIMAL_FLOATING`*
- *`HEXADECIMAL_FLOATING`*

#### *`x_FLOATING`*

- *`x_INTEGER`*`(\.`*`x_DIGIT`*`+)?(`*`EXP_SEPARATOR`*`[+\-]?`*`INTEGER`*`)?`*`WS`*

#### *`INTEGER`*

- *`BINARY_INTEGER`*
- *`OCTAL_INTEGER`*
- *`DECIMAL_INTEGER`*
- *`HEXADECIMAL_INTEGER`*

#### *`x_INTEGER`*

- *`x_HEADER`*`(0|(`*`x_DIGIT(except 0) x_DIGIT`*`*))`

#### *`DIGITs`*
- *`BINARY_DIGIT`*
 - `[0-1]`
- *`OCTAL_DIGIT`*
 - `[0-7]`
- *`DECIMAL_DIGIT`*
 - `[0-9]`
- *`HEXADECIMAL_DIGIT`*
 - `[0-9A-Fa-f]`

#### *`HEADERs`*
- *`BINARY_HEADER`*
 - `0b`
- *`OCTAL_HEADER`*
 - `0`
- *`DECIMAL_HEADER`*
 - `(none)`
- *`HEXADECIMAL_HEADER`*
 - `0x`

#### *`EXP_SEPARATOR`*

- `[EePp@]`

#### *`INVOCATION`*

- *`FUNCTION`*
 - *`FUNCTION_NAME WS`*`\(`*`EXPRESSION`*`(,`*`EXPRESSION`*`)*\)`*`WS`*
- *`CONSTANT`*
 - *`CONSTANT_NAME`*`(`*`WS`*`\(`*`WS`*`\))?`*`WS`*

#### *`UNARY_OPR`*

- `[\+\-]`*`WS`*

#### *`BINARY_OPR`*

- `[\+\-\*\/\%\^]`*`WS`*

#### *`NAMEs`*

Follows `[A-Za-z][_0-9A-Za-z]*`.

- *`FUNCTION_NAME`*
 - Supported functions: `abs`, `mod`, `ma`, `remainder`, `max`, `min`, `dim`, `exp`, `exp2`, `expm1`, `ln`, `log2`,
 `lg`, `ln1p`, `pow`, `sqrt`, `cbrt`, `hypot`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`, `sinh`, `cosh`,
 `tanh`, `asinh`, `acosh`, `atanh`, `erf`, `erfc`, `tgamma`, `lgamma`, `ceil`, `floor`, `trunc`, `round`.
- *`CONSTANT_NAME`*
 - Supported constants: `e`, `pi`.

#### *`WS`*

- `[\t ]*`

### *`ERROR_MESSAGE`*

Any *`ERROR_MESSAGE`* is printed to `stderr`.

 - `In (FILE), at (LINE):(COLUMN), (ERROR_REASON).`
 - `(FILE)`
  - The name of current input file, while `(stdin)` stands for `stdin`.
 - `(LINE)`
  - The current line number.
 - `(COLUMN)`
  - The current column number.
 - `(ERROR_REASON)`
  - A string to describe the error.

