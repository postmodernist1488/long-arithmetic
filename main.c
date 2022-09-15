#include <stdio.h>
#include <stdint.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define BASE 10
#define BIG_INT_CAP 1000

char g_digit_lut[BASE] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
#define itoch(num) g_digit_lut[num]
#define min(a, b) (a < b) ? a: b
#define emod(a, b) (((a) % (b)) + (b)) % (b)

int sign(int x) {
    return (x > 0) - (x < 0);
}

typedef struct {
    int8_t sign;
    int last;
    int array[BIG_INT_CAP];
} Big_int;

void int_to_big_int(int value, Big_int *big_int) {
    if ((big_int->sign = sign(value)) < 0) {
        value = -value;
    }
    for (big_int->last = -1; value > 0; value /= BASE) {
        big_int->array[++big_int->last] = value % BASE;
    }
}

void print_big_int(Big_int *big_int) {
    if (big_int->sign < 0) {
        putchar('-');
    }
    for (int i = big_int->last; i >= 0; i--) {
        assert(abs(big_int->array[i]) < BASE);
        putchar(itoch(big_int->array[i]));
    }
}

bool big_int_greater_than_abs(Big_int *a, Big_int *b) {
    if (a->last > b->last) {
        return true;
    }
    else if (a->last < b->last) {
        return false;
    }
    else {
        for (int i = a->last; i >= 0; i--) {
            //TODO: use memcmp
            if (a->array[i] < b->array[i])
                return false;
            else if (a->array[i] < b->array[i])
                return true;
        }
        //equal
        return false;
    }
}

bool big_int_greater_than(Big_int *a, Big_int *b) {
    if (a->last > b->last || a->sign > b->sign) {
        return true;
    }
    else if (a->last < b->last || a->sign < b->sign) {
        return false;
    }
    else {
        for (int i = a->last; i >= 0; i--) {
            //TODO: use memcmp
            if (a->array[i] < b->array[i])
                return false;
            else if (a->array[i] < b->array[i])
                return true;
        }
        //equal
        return false;
    }
}

Big_int big_int_add(Big_int *a, Big_int *b) {
    int carry = 0;
    Big_int result = {0};

    Big_int *shorter = a, *longer = NULL;
    if (a->last < b->last) {
        shorter = a;
        longer = b;
    }
    else if (a->last > b->last) {
        shorter = b;
        longer = a;
    }
    int i;
    int invert = 1;
    if (a->sign == b->sign && b->sign == -1) {
        result.sign = -1;
        a->sign = 1;
        b->sign = 1;
    } else if (a->sign != b->sign) {
        if (big_int_greater_than_abs(a, b))
            result.sign = a->sign;
        else 
            result.sign = b->sign;
        invert = result.sign;
    }
    for (i = 0; i <= shorter->last; i++) {
        int s = invert * (a->array[i] * a->sign + b->array[i] * b->sign) + carry;
        result.array[i] = emod(s, BASE);
        if (s < 0)
            carry = -1;
        else
            carry = s / BASE;
    }
    while (longer != NULL && i <= longer->last) {
        carry = carry + longer->array[i];
        result.array[i] = emod(carry, BASE);
        if (carry < 0)
            carry = -1;
        else
            carry /= BASE;
        i++;
    }
    if (carry != 0) {
        assert(abs(carry) < BASE);
        result.array[i++] = abs(carry);
    }
    while (result.array[--i] == 0 && i > 0);
    result.last = i;
    return result;
}

Big_int int_big_int_mul(Big_int big_int, int n) {
    int carry = 0;
    for (int i = 0; i <= big_int.last; i++) {
        int64_t d = big_int.array[i] * (int64_t) n + carry;
        big_int.array[i] = d % BASE;
        carry = d / BASE;
    }
    while (carry > 0) {
        if (big_int.last + 1 >= BIG_INT_CAP) {
            fprintf(stderr, "Big int overflow detected!");
            exit(1);
        }
        big_int.last++;
        big_int.array[big_int.last] = carry % BASE;
        carry /= BASE;
    }
    return big_int;
}

Big_int big_int_mul(Big_int *a, Big_int *b) {
    (void) a;
    (void) b;
    assert(0 && "Not Implemented");
}

void test_add(int a, int b) {
    Big_int ba, bb;
    
    int_to_big_int(a, &ba);
    int_to_big_int(b, &bb);

    Big_int res = big_int_add(&ba, &bb);
    printf("%10d + %10d = ", a, b);
    print_big_int(&res);
    printf(" Should be: %d\n", a + b);
}

void test_mul(int big_int, int b) {
    Big_int ba;
    
    int_to_big_int(big_int, &ba);

    Big_int res = int_big_int_mul(ba, b);
    printf("%10d x %10d = ", big_int, b);
    print_big_int(&res);
    putchar('\n');
}

int main (void) {
    test_add(-1234567, 12345670);
    test_add(-12345670, 1234567);
    test_add(-1234, 12345670);
    test_add(1234, 12345670);
    test_add(12342130, 12345670);
    test_add(12342130, 0);
    test_add(-1234, -7890);
    test_add(12342130, -12342130);
    test_add(1000, -10);
    // 1000
    // -990
    test_mul(1000, 1234);
    test_mul(1243989734, 1243989734);
    putchar('\n');

    return 0;
}
