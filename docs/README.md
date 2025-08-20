# JACLang 2025.1.1 Specification

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
For the same reason, references may not be stored within memory, or be returned from a function. They can, however be passed to functions.

# IDEAS (WIP)

## Compile-time groups

Syntax signifying compile-time operations or expansions.

```
$(x) -- macro-expansion of macro 'x'
?(obj.type) -- metadata-expansion of the type string of 'obj'
#(inline) -- compiler hint
```

The C/C++ <code>\_\_LINE\_\_</code> and <code>\_\_FILE\_\_</code> type of functionality would be given to the user
as metadata-expansion. You're querying and expanding the metadata of the compilation process.

```
?(line) -- expands to the line number
```

## Macros

```rust
macro loop -> { while true }

$(loop) {
    print("Hey!");
}
```

## Compile-time reflection

<code>?</code> in front of the expansion group accesses type metadata, the different fields of which
can be accessed inline, at compile-time. Such as <code>?(obj.name) -- object structure name</code> 
The checking of equality between two types is allowed, and a compile-time operation.
Resulting boolean value is compile-time constant, and can be optimized away.

Type identifiers are automatically treated as 'objects' of type metadata. This can be done, because everything is known about the structure at compile time. So the following is allowed.

```rust
struct Foo {
    u32: a => { return a; }
    u8: b;

	new() -> Foo {
		return Foo {};
	}
}

if Foo = u32 {
    print("They are equal.");
}
```

Compile-time constant array metadata, such as structure fields, could possibly be inlined and materialized onto the stack at the place of inlining. That way one could obtain a list of structure fields at runtime (still created at compile-time), and iterate over them. This would allow for automatic serialization of arbitrary structures by recursing and serializing the leaves (intrinsic types).


## Structures

Special getter-syntax. The usage of direct setters is discouraged, and thus we don't have similar syntax for them. The getters would take the form of

```rust
struct Structure {
	u8: field => {...}
}
```

where the block to the right of the special arrow is the getter code. This syntax minimizes boilerplate, but still allows for an expressive and powerful getter logic. So the most basic getter would be

```rust
struct Structure {
	u8: field => { return field; }
}
```

which would be called, whenever the field is accessed outside of the class <code>object.field -- calls the getter</code>. As similar setters are not allowed, the writing of meaningful and operative methods is encouraged instead. Omitting the getter makes the field private to the class. So

```rust
struct Structure {
	u8: public_field => { return field; } -- public
	bool: private_field; -- private
}
```

## Template functions

Minimalist syntax. Simple template type declaration after the function identifier marks the function as templated.

```cpp
example_function<T>(T: argument) {...} -- 'T' is a templated type
```

We could leverage the compile-time reflection system to create a powerful constraint system. With the addition of the keyword <code>requires</code>, a constrained template function would become

```cpp
example_function<T>(T: argument)
	requires [compile-time constant boolean expression]
{...}
```

Compile-time constant functions returning a boolean value could be written by the user or a library, and those could be chained after the <code>requires</code> clause, just like a normal boolean expression. If the expression evaluates to false at compile-time, an error could be emitted. An optional <code>else</code> clause could even be added, allowing the user to report custom error messages, like so

```cpp
example_function<T>(T: argument)
	requires has_field(argument, "x")
	else "error: provided template type " T.type " didn't have field 'x'."
{...}
```

## Unit system, imports / exports

Files are treated as self-contained and built compilation units. Each compilation unit outputs an build metadata file that contains the full export table, with enough information that proper type-checking can be done. Then, the desired symbols can be 'imported' to the unit with the following syntax:

```rust
from "[filepath]" use [namespace]::{symbol1, symbol2, *};
```

You can import specific symbols, or wildcard all of them. All imports must be migrated under a new namespace that hasn't been used in the unit before. This way symbol collisions are impossible, as unpacking to 'global' namespace is forbidden. It's the importers responsibility to namespace their symbols as they see fit.

Namespacing is implemented by name mangling, which incorporates a per-unit UUID. Each unit includes in its own metadata file a unique hash, which is then incorporated into the mangled names of the exported symbols.

If the filepath provided begins with '.' or '..', the path is treated as relative to the current source file. If not, the system directories (/include, etc.) and the CLI-provided include-directories (--include-dirs), are searched.

## Defer and destructors

The `defer` keyword defers the execution of the following statement till the scope-exit. This allows for clean and robust cleanup logic.

```go
File: file := File{};
defer file.close();

if a < b {
	return 3; // called here
}
else {
	return -999; // and here
}
```

Structure destructors are simply syntactic sugar over the deferring system. Struct methods qualified by `defer` are treated as if you'd deferred the method right after the creation of the object.

```go
struct Structure {
	defer func cleanup() { ... }
} 
```

The method 'cleanup' is called automatically at object destruction (scope exit), and is functionally equivalent, and treated as, following:

```go
Structure: obj := Structure{};
defer obj.cleanup();
```