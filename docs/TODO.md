# JACLang feature plan

- [ ] expressions
  - [x] binary expressions
    - [x] parentheses
    - [x] order of operations

  - [ ] unary expressions
    - [ ] prefix
      - [ ] increment
      - [ ] decrement
      - [ ] negation (-)
      - [ ] positivisation (+)
      - [ ] logical not

    - [ ] affix
      - [ ] array indexing

- [ ] stack variables
  - [x] definition
  - [ ] assignment

- [ ] scopes
  - [ ] variable lifetimes

- [ ] control flow
  - [ ] if
    - [ ] elif
    - [ ] else

  - [ ] match-case
  - [ ] while
  - [ ] for

- [ ] functions
  - [ ] definition
  - [ ] calls
  - [ ] return

- [ ] macros
  - [ ] macro variables
  - [ ] macro functions

- [ ] memory management
  - [ ] references
  - [ ] pointers (maybe?)
    - [ ] null deref checking (debug mode only)

  - [ ] arrays
    - [ ] bounds checking (debug mode only)

- [ ] static typesystem
  - [ ] type checking
  - [ ] type sizes

- [ ] float math

- [ ] project file management
  - [ ] symbol visibility
    - [ ] public
    - [ ] private

  - [ ] 'from {file} use *'
    - [ ] wildcard import (all public symbols)
    - [ ] import specific public symbols by name
    - [ ] 'migrate' symbols under new namespace
  
- [ ] standard library
  - [ ] print(str)
  - [ ] exit(int)

- [ ] release mode
  - [ ] optimizations
