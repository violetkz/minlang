# minlang
The 'minlang' is minimal program language just for fun. :laughing:

## :four_leaf_clover:about *minlang*
*minlang* looks like C language except without strict data type declaration.
so it is a dynamic language.
currently, it support Integer calculation,string operation and dynamic array.
and also be able to define own function. 

## :airplane: try it.
first, compile src code. since *minlang* depend on c++11 standard, GCC need
be at least 4.7 or later in Linux/Cgywin/mingw platform. the version of CLang
need be 2.9+ in MacOS
    ```shell
    $ make                        #for Linux/cygwin
    $ make -f makefile.mingw      #for mingw32 
    $ make -f makefile.mac        #for MacOS
    ```
then, play it. :tada:
    ```shell
    $ ./minlang -f test/qsort.ns
    ```

## :zap: examples:

* see hello to world.  :sunglasses:
    ```c
    main { 
        print("Hello World!");
    }
    ```

* simple logic/math calculation
    ```c
    main {
        i = 9;
        while (i > 0) {
            if (i % 2 == 0) {
                print(i);
            }
            i = i - 1;
        }
    }
    ```

* Dynamic Array
    ```c
    ## array test
    main {
        a = [99, "string", 88];

        x = a[0] + 1;
        print(x);  ## output: 100

        a[1] = "ss";
        print(a); ## output: [99,ss,88,]
    }
    ```

    anthoer one:
    ```c
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

* Oh, Fibonacci! :fire:

    ```c
    def fibonacci (n) {
        if (n == 0 or n == 1) {
            return n;
        }
        return fibonacci(n - 1) + fibonacci(n - 2);    
    }

    main {
        print("fibonacci(30):", fn(30));
    }
    ```

* cool :bangbang: let's make a quick-sort.

    ```c
    def parition(arr, start_idx, end_idx) 
    {
        mid_val = arr[(start_idx + end_idx)/2];

        i = start_idx;
        j = end_idx;
       
        while (i <= j) {
            while (arr[i] < mid_val) {
                i = i + 1;
            }
            while (arr[j] > mid_val) {
                j = j - 1;
            }
            if (i <= j) {
                tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                i = i + 1;
                j = j - 1;
            }
        }
        return i;
    }

    def qsort(arr, start_idx, end_idx) {
        index = parition(arr, start_idx, end_idx);
        if (start_idx < index - 1) {
            qsort(arr, start_idx, index - 1);
        }
        if (index < end_idx) {
            qsort(arr, index,  end_idx);
        }
    }
    
    main {
        testcases = [
            [1],
            [3,2,9],
            [9923,281,1,8342,2,72,63264,4576],
            [80, 51, 41, 38, 75, 91, 88, 88, 52, 96, 88, 99, 71, 95, 35, 28]
        ];

        for tc in testcases {
            print("sorting :", tc);
            qsort(tc, 0, tc.len() -1);
            print("sorted  :", tc);
        }
    }
    ```

## :four_leaf_clover:why do I make *minlang*. 

* It is realy funny.:rocket:
* For better understanding program language.:feet:

*minlang* created by Flex, Bison and Cpp. 
