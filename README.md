# Excerpt

## Project Structure

- `include/`: Header files.
  - `frontend/`: The frontend part of the compiler.
  - `backend/`: The backend part of the compiler.
  - `utils/`: Tools, i.e logging and arg parsing.
- `src/`: Source code files.
  - `frontend/`: The frontend implementations.
  - `backend/`: The backend implementations.
- `tests/`: Unit tests.

## Installing dependencies/developer environment

To handle dependencies, I use `nix` along with flakes.

```sh
nix develop .#verte-dev --impure
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
verte-build
```

The output of the build will go into `build/`

Testing:

```sh
verte-tests
```
