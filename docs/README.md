# JACLang 1.0 Specification

## Contents
1. [Introduction](#intro)
2. [Philosophy](#philosophy)
3. [Language Tokens](#tokens) \
	3.1. Keywords \
	--- 3.1.1. [Reserved](#reserved) \
	--- 3.1.2. [Primitive Types](#types) \
	3.2. [Identifiers](#identifiers) \
	3.3. Literals \
	--- 3.3.1. [Numeric Literals](#num_lit) \
	--- 3.3.2. [String Literals](#str_lit) \
	3.4. Operators \
	--- 3.4.1. [Arithmetic](#arithmetic_ops) \
	--- 3.4.2. [Logical](#logical_ops) \
	--- 3.4.3. [Other](#other_ops)
4. [Language Constructs](#language) \
    4.1. [Array](#array) \
    4.2. [Reference](#reference)
 
## Introduction <a name=intro></a>
<mark style="background-color: blue">NOTE: JACLang is a hobby project, made for learning and personal interest purposes</mark>

This is the formal specification for the first edition of JACLang: a statically-typed, explicit and compiled programming language. It takes inspiration from multiple other programming languages,
mainly [Python](https://www.python.org), [Rust](https://www.rust-lang.org), [C](https://wikipedia.org/wiki/C_(programming_language)) and [C++](https://isocpp.org/).

## Philosophy <a name=philosophy></a>
The main promise and ideological angle of JACLang is such:
> Explicity over implicity. Whichever can be made implicit and hidden, shall not be.
> Minimal amount of black boxes: All you see is all she wrote.

In practice this means strictly forbidding implicit behavior, such as:

Default function parameters *(example in C++)*
```cpp
void function(int argument = 666);
```
Implicit casting *(example in C)*
```c
int integer = 3.141592f;
```

## Language Tokens <a name=tokens></a>
### Keywords - Reserved <a name=reserved></a>
| identifier | description |
| :--------- | :---------- | 
| mut | [qualifier] allow variable mutation |
| global | [scope] make symbol global |
| local | [scope] make symbol local |
| if | [flow] if statement |
| elif | [flow] else if statement |
| else | [flow] else statement |

### Keywords - Primitive Types <a name=types></a>
Short and descriptive type names. Primitive types MUST be the size they are advertised
as, i.e a <code>u32</code> must be EXACTLY 32 bits wide.

| identifier | description |
| :--------- | :---------- | 
| bool | primitive boolean type; 1 byte wide. |
| u8 | primitive unsigned byte type; 1 byte wide. |
| u16 | primitive unsigned word type; 2 bytes wide. |
| u32 | primitive unsigned double word type; 4 bytes wide. |
| u64 | primitive unsigned quad word type; 8 bytes wide. |
| i8 | primitive signed byte type; 1 byte wide. |
| i16 | primitive signed word type; 2 bytes wide. |
| i32 | primitive signed double word type; 4 bytes wide. |
| i64 | primitive signed quad word type; 8 bytes wide. |
| f32 | primitive IEEE-754 floating-point single precision type; 4 bytes wide. |
| f64 | primitive IEEE-754 floating-point double precision type; 8 bytes wide. |

### Identifiers <a name=identifiers></a>
The rules for valid identifiers are as follows:
>  Must be composed of the following types of characters \
> -- Alphabetic character \
> -- Numeric character \
> -- Underscore \
\
> **EXCEPT**
>  The first character of the identifier must either be \
> -- Alphabetic character \
> -- Underscore

### Literals - Numeric Literals <a name=num_lit></a>
Numeric literals are compile-time constant, literal numeric values. \
They are interpreted as a single and continuous number, with an optional decimal (.) point. \
In the case of a second decimal point within the number, the remaining number will be ignored. I.e. the number <code>16.74.072</code> becomes <code>16.74</code>.

Numeric literals can have one of the [primitive types](#types) postfixed after themselves. \
This helps in type checking variable assignment and function parameter passing, as the types must match.
```rust
u8: variable := 255u8;
```

In numeric literals where the number body, and the postfix type contradict each other, the true type of the number is decided by the postfix type. \
I.e. if the numeric literal in question was <code>0.32u8</code>, the type is interpreted as <code>u8</code>.

The underscore (\_) can be used as an ignored separator character inside the numeric literal.
```rust
u32: million := 1_000_000_u32;
```

### Literals - String Literals <a name=str_lit></a>
String literals are interpreted as the characters enclosed by a pair of parentheses ("). \
The storage type of a string literal is an [array](#array) of unsigned bytes. \
All string literals are allocated on the heap.

### Operators - Arithmetic Operators <a name=arithmetic_ops></a>

| identifier | description |
| :--------- | :---------- | 
| + | addition |
| - | subtraction |
| * | multiplication  |
| / | division |
| += | add to |
| -= | subtract from |
| \*= | multiply by |
| /= | divide by |

### Operators - Logical Operators <a name=logical_ops></a>

| identifier | description |
| :--------- | :---------- | 
| ! | logical not |
| = | logical equals |
| != | not equals |
| & | logical and |
| \| | logical or |
| < | less than |
| > | greater than |

### Operators - Other Operators <a name=other_ops></a>

| identifier | description |
| :--------- | :---------- | 
| \|x\| | length of | 
| x[] | array indexing | 
| &x | reference of |

## Language Constructs <a name=language></a>

### Array <a name=array></a>
Arrays must be continuous in memory. \
Arrays must have a preceding header directly before the memory pointer, which stores the size and data type stride \
of the array. In this way the JACLang arrays are identical to C arrays, in that they are just a pointer to continuous memory, but retain their length and type information.
Thus the whole array must have the following memory layout:

| size | stride | array memory |
| :--- | :----- | :----------- |
| 8 bytes | 8 bytes | n bytes |

Array indexing is started from the beginning of the user memory block.

The ['length of'](#other_ops) operator must return the correct header-stored size of the array. \
Array indexing must be bounds-checked in debug mode. \
Negative indexes must be correctly wrapped around, according to the compile-time length of the array.

Arrays are interpreted as an optionally recursive pairing of type and size inside enclosed square brackets.
```rust
[u32, 4]: array1 := { 1, 2, 3, 4 };
[[u8, 2], 2]: array2 := { { 1, 2 }, { 3, 4 } };
```

### Reference <a name=reference></a>
References to objects must ALWAYS be valid. To uphold this contract, only the stack may be referenced. \
For the same reason, references may not be stored within memory, or be returned from a function. 

### Promise <a name=promise></a>
A symbol must always be initialized to a value. For that reason you could not create two functions that call each other. \
To achieve this behavior, you need to create a promise. A promise is a contract that promises to define a symbol elsewhere. \
That is, if you wanted to 'forward declare' a variable, you'd do it the following way:

```rust
promise u32: mut variable;

u32: main([u8]: argv) {
    return variable; 
}
```

