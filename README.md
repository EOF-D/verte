# Excerpt

## Project Structure

- `include/`: Header files.
  - `excerpt/`: Project-specific header files.
  - `excerpt_utils/`: Tools, i.e logging.
- `src/`: Source code files.
- `tests/`: Unit tests.

## Installing dependencies/developer environment

To handle dependencies, I use `nix` along with flakes.

```sh
nix develop .#excerpt-dev --impure
```

Note: We use `--impure` because I am using this along with `devenv`
Additionally, if you have `direnv` you may run:

```sh
direnv allow
```

To automatically enter the environment everytime you enter the directory.

## Building & Testing

**note**: (inside of the environment)

Building:

```sh
excerpt-build
```

The output of the build will go into `build/`

Testing:

```sh
excerpt-tests
```
