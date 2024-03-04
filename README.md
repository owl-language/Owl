# OwlInterpreter
 An AST walking interpreter for Owl.
 
 Owl is a statically typedv, procedural language with algol/wirthian syntax.
 It has support for recursion, looping, variables, arrays. right now the type system
 only works for integers but the current number 1 task is expanding the typeing.

There are two ways of running owl scripts, via the command line interpreter, and via the interactiver debuger
they can be found in the tools subdir

       tools/cli_interpreter.cpp - run scripts from comman line as in the examples below
       tools/repl_interpreter.cpp - more of an interactive debugger, REPL functionality on the way ;)

Some features of Owl are highlighted below. All code samples are real, working examples.

## Scoping

      program 'scope';
      begin
         let t: int := 13;
         func scoped(x: int) begin
            let t: int := 42;
            print t;
            print (x + t);
        end;
        scoped(t);
        print (t);
     end
     
     max@MaxGorenLaptop:~/GitHub/OwlInterpreter$ ./owlcli owlcode/scope.owl
     42
     55
     13
     max@MaxGorenLaptop:~/GitHub/OwlInterpreter$

## Iteration

     program 'euclids';
     begin
        let a: int := 72;
        let b: int := 42;
        while (a != b) begin
            if (b < a) then
               a := a - b;
            else
               b := b - a;
            end;
        end;
        print a;
     end
     max@MaxGorenLaptop:~/GitHub/OwlInterpreter$ ./owlcli owlcode/euclid.owl
     6
     max@MaxGorenLaptop:~/GitHub/OwlInterpreter$
     
## Recursion, user input

    program 'factorial';
    begin
        let x: int := 0;
        func factR(x: int) begin
           let t: int := 1;
           if (x <= 1) then
              return 1;
           else
              t := (x * factR(x-1));
           end;
           return t;
        end;
        input n;
        x := factR(n);
       print x;
    end
    max@MaxGorenLaptop:~/GitHub/OwlInterpreter$ ./owlcli owlcode/fact.owl
    5
    120
    max@MaxGorenLaptop:~/GitHub/OwlInterpreter$
    
## Arrays

    program 'arrayEx';
    begin
       let i: int := 0;
       let x[5]: int;
       while (i < 5) begin
          x[i] := i+1;
          i := i + 1;
       end;
       i := 0;
       while (i < 5) begin
          print x[i];
          i := i + 1;
       end;
    end
    max@MaxGorenLaptop:~/GitHub/OwlInterpreter$ ./owlcli owlcode/array.owl
    1
    2
    3
    4
    5
    max@MaxGorenLaptop:~/GitHub/OwlInterpreter$
      
## Bubblesort
(A bad sorting algorithm, a good display of Owl)
    program 'bubblesort';
    begin
       let c: int := 1;
       let d: int := 2;
       let ds: int := 0;
       let i: int := 1;
       let x[10]: int;

       func swap(v: int, u: int) begin
          let t: int := 0;
             t := x[v];
          x[v] := x[u];
          x[u] := t;
          ds := 1;
       end

       func showArr(n: int) begin
           let k: int := 1;
           k := 1;
          while (k < n) begin
            print x[k];
            k := k + 1;
          end;
       end

       func sortPass(m: int, n: int) begin
          c := m;
          d := c + 1;
          while (d < n) begin
             if (x[d] < x[c]) then
                swap(c, d);
             end;
             c := c + 1;
             d := c + 1;
          end;
       end

       func sort(l: int) begin
          ds := 0; {* did swap flag, once sortPass completes without setting to 1, were done *}
          sortPass(1, l);  {* do bubble pass *}
          end;
          if (ds = 0) then
            return 0;
          else
            sort(l);
          end;
       end

      {* main program starts here *}
      
       i := 1;
       while (i < 10) begin      
          x[i] := rand(100);
          i := i + 1;
       end;
    
      showArr(10);
      sort(10);
      showArr(10);
    end
 
