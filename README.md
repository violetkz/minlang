# minlang
The 'minlang' is minimal program language just for fun.

## about *minlang*
*minlang* looks like C language except without strict data type declaration.
so it is a dynamic language.
currently, it support Integer calculation,string operation and dynamic array.
and also be able to define own function. 

## examples:

* see hello to world.  :-)
    ```
    main { 
        print("Hello World!");
    }
    ```

* simple logic/math calculation
    ```
    main {
        i = 9;
        while(i > 0) {
            if ( i % 2 == 0) {
                print (i);
            }
            i = i - 1;
        }
    }
    ```

* Array
    ```
    ## array test
    main {
        a = [99, "string", 88];

        x = a[0] + 1;
        print(x);  ## output: 100

        a[1] = "ss";
        print (a); ## output: [99,ss,88,]
    }
    ```

    anthoer one:
    ```
    main {
        a = [];
        i = 0;
        while (i < 99) {
            a.append(i);
        }

        print("the length of array a:", a.len());
        
        re = a.find(99);
        print("the index of 99 in the array a:", re);
    }
    ```

* Oh, Fibonacci! haha

    ```
    def fibonacci (n) {
        if (n == 0 or n == 1) {
            return n;
        }
        return fibonacci(n - 1)+ fibonacci(n-2);    
    }

    main {
        print("fibonacci(30):", fn(30));
    }
    ```

## why do I make *minlang*. 

* It is realy funny.
* For better understanding program language.

*minlang* created by Flex, Bison and Cpp. 
