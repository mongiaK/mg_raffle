use String;

fn run(mut x :i32) -> i32 {
    x = 7;
    return x;
}

struct girl;
struct boy;

#[derive(Debug)]
enum Sex {
    girl,
    boy,
}

fn Sex_Choose(arg: &Sex) -> u8 {
    match arg {
        Sex::girl => 1,
        Sex::boy => 0,
    }
}

fn main() {
    let mut a = 10;
    a = run(a);
    println!("{a}");
    
    const MAX_NUM :char = 'P';
    println!("{MAX_NUM}");
 
    let tup: (i32, i64, f64) = (1, 2, 1.1);
    println!("{}", tup.2);

    let arr: [i32; 5] = [1, 2, 3, 4, 5];
    println!("{}", arr[0]);

    if arr[3] > 4 {
        println!("arr 3 great than 4"); 
    }else {
        println!("arr 3 less than 4");
    }

    let mut str = String::from("hello");
    str.push_str(", world");
    println!("{str}");
}
