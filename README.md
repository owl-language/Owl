# OwlInterpreter
 An AST walking interpreter for Owl

# scoping

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
