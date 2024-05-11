# Excerpt

## Project Structure

- `include/`: Header files.
  - `excerpt/`: Project-specific header files.
  - `excerpt_utils/`: Tools, i.e logging.
- `src/`: Source code files.
- `tests/`: Unit tests.

## Building

```sh
nix develop .#excerpt-dev --impure
excerpt-build
```

The output of the build will go into `build/`

## Running tests

```sh
nix develop .#excerpt-dev --impure
excerpt-tests
```
