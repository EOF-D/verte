fn add(x: int, y: int) -> int {
  return x + y;
}

fn main() -> int {
  const foo: int = 10;
  const bar: int = 20;

  printf("foo: %d\n", foo);
  printf("bar: %d\n", bar);

  printf("%d + %d = %d\n", foo, bar, add(foo, bar));
  return 0;
}
