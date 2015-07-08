## Synopsis

This project was a implentation of master transactional log file, in other words, a library to store in safe way information about transaccions. This information was stored in secure way, to do it use a crc32 code for each register stored. This project could be used if you need to store sequencial transaccions (not need to insert sequencial, but holes in transaccion id have a cost). Implements two indexes on for transaccional id, but not implements a hash o b-tree, implements a table, for this reason holes have a cost. The second index was for logs, used normaly if need to replicate this file in other host.

## Code Example

```C++
int main ( int argc, char *argv[] )
{
  printf("test2: %d\n", argc);
}
```

## Motivation

This project born in 2013, used internally and not availaible in github. At this year proyect was available, and we continue to make updates. This project born to resolve a specific problem, in way to find the best performance solution. We saw this kind of file in a lot of transaccionals systems, perhaps not new systems, but very good performance systems.

## Installation

To use this project only need library libsafestorage.so and safe_storage.h header file. To compile project only need execute make on root of project.

## API Reference

Working on this section

## License

This project have a very flexible license, a MIT license, read LICENCE file to more information
