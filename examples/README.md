# Examples

Runnable example programs demonstrating the library's usage.
Organized into categories under `suite/` (see `suite/README.md` for
what each category covers). Every example registers itself
automatically at startup — no manual wiring needed when a new example
file is added.

## Building

Configure with examples enabled and build:

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=ON -DBUILD_BENCHMARKS=OFF -DBUILD_REGRESSION=OFF
cmake --build build
```

This produces a single `examples` executable under `build/`.

## Usage

Run every registered example, then print pass/fail stats:

```
./build/examples
```

List all examples, grouped by category — useful when you don't
remember an example's id or exact file name:

```
./build/examples list
```

Run every example in one category, e.g. everything under `quickstart`:

```
./build/examples quickstart
```

Run a single example by its id, e.g. the first quickstart example:

```
./build/examples Q1
```

Run a single example by its file name instead of its id:

```
./build/examples basic_behavior
```

Matching is case-insensitive. Category and file-name arguments are
matched against the same `list` output, so run `./build/examples
list` first if you're not sure what's available.
