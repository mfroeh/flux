<p align="center">
<img src="logo.png" alt="Flux logo" height="200">
</p>

# Flux

An imperative, statically typed toy programming language with classes, type inference and some other fun features.

## How to use
Try it out with nix: `git clone git@github.com:mfroeh/flux && cd flux && nix run . -- examples/classes.fl && clang a.out -o out && ./out`
* Builds the compiler and runs it to compile `examples/classes.fl`
* Links the produced object file `a.out` using clang
* Runs the binary `out`

Whenever developing
* Enter devshell: `nix develop`

One time dev setup
1. Build compiler: `mkdir build && cd build && cmake .. && ninja -j 8`
2. Make work with clangd: `cd .. && ln -s build/compile_commands.json .`

Make package and run
* Build and run package: `nix build && result/bin/flux` or `nix run`

## Features
* [Classes](examples/classes.fl)
```
class Point { 
    x: i64; 
    y: i64; 
}

class Rectangle { 
    topleft: Point;
    bottomright: Point; 

    height(): i64 => this->bottomright.y - this->topleft.y;

    width() => this->bottomright.x - this->topleft.x;

    area() => this->height() * this->width();

    contains(p: Point*): bool {
        ret p->x >= this->topleft.x && p->x <= this->bottomright.x &&
            p->y >= this->topleft.y && p->y <= this->bottomright.y;
    }
}

main(): i64 {
    let p1 = Point { x: 0, y: 0 };
    let p2: Point;
    print "p2: %d %d " p1.x, p2.x;

    p2.x = 10;
    p2.y = 10;

    let r = Rectangle { topleft: p1, bottomright: p2 };

    let height = r.height();
    let width = r.width();
    let area = r.area();

    let p = Point { x: 5, y: 5 };
    let contains = r.contains(&p);

    print "height: %d, width: %d, area: %d, contains: %d" height, width, area, contains;
    ret 0;
}
```

* [Multi-dimensional arrays](examples/arrs.fl)
```
main(): i64 {
    let board: i64[8, 8, 8];
    board[1, 2, 3] = 44;
    print "%d " board[1, 2, 3];

    // remaining elements are default initialized
    let arr: f64[8] = [2.5, 2.6];
    print "%.2f " arr[3];
    ret 0;
}
```

* [Pointers](examples/pointers.fl)
```
sum(arr: i64[4]*): i64 {
    let sum = 0;
    for (let i = 0; i in [0, 4); i += 1;) -> sum += *arr[i];
    ret sum;
}

buildsquares(arr: i64*[4]*): i64*[4]* {
    for (let i = 0; i < 4; i += 1;) {
        let ptr = *arr[i];
        *ptr *= *ptr;
    }
    ret arr;
}

main(): i64 {
    let arr = [1, 2, 3, 4];
    let arrPtrs: i64*[4] = [&(arr[0]), &(arr[1]), &(arr[2]), &(arr[3])];
    let ptrptr = &(arrPtrs[2]);
    print "%d " sum(&arr);
    print "%d==%d==%d " *squares(&arrPtrs)[2], arr[2], **ptrptr;
    ret 0;
}
```

* [Variable and function shadowing](examples/shadowing.fl)
```
add(a: i64, b: i64) => a + b;

main(): i64 {
    let a = 3.1415;
    print "a: %f " a;
    let a = true;
    print "a: %d " a;
    let a = "hello";
    print "a: %s " a;

    print "add(1, 2): %d " add(1, 2);
    add(a: i64, b: i64): f64 => a * b;
    print "add(1, 2): %f " add(1, 2);
    ret 0;
}
```

* [Type inferrence](examples/infer.fl)
```
add(a: i64, b: i64) => a + b;

main(): i64 {
    let res = add(1, 2);
    print "%d " res;
    ret 0;
}
```

* Expressions [expression](grammar/FluxParser.g4)

* Builtin types [types](grammar/FluxParser.g4)

* Includes

```
// file: math.fl
class Point {
    x: i64;
    y: i64;
}

add(a: i64, b: i64) => a + b;
```

```
// file: main.fl
incl ./math.fl;

main(): i64 {
    let p = Point { x: 1, y: 2 };
    print "add(p.x, p.y): %d " add(p.x, p.y);
    ret 0;
}
```

