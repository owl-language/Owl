# OwlInterpreter
 An AST walking interpreter for Owl.
 
 Owl is a statically typedv, procedural language with algol/wirthian syntax.
 It has support for recursion, looping, variables, arrays. right now the type system
 only works for integers but the current number 1 task is expanding the typeing.

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
     
## Recursion
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
