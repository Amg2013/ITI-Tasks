import 'dart:io';

int findGCD(int a, int b) {

  while (b != 0) {
    int temp = b;
    b = a % b;
    a = temp;
  }
  return a;
}

void main() {
  // Read first number
  stdout.write('Enter first number: ');
  int num1 = int.parse(stdin.readLineSync()!);

  // Read second number
  stdout.write('Enter second number: ');
  int num2 = int.parse(stdin.readLineSync()!);

  // Calculate GCD
  int gcd = findGCD(num1, num2);

  print('The GCD of $num1 and $num2 is $gcd');
}

///    12      8   
///  8 >>>>>   2  4  
///  12 >>>> 2  3  4 6 
///// 4   2 

/// 10  5 
///  10 >> 1   2  5  
/// 5 >> 1  5 
