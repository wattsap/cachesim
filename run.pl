#!/usr/bin/perl

#names of the input files
@files = ("simple.din", "008.espresso.din", "013.spice2g6.din"); 
@tests = ("-E 4 -S 16 -B 256", 
          "-E 8 -S 32 -B 512",
          "-E 16 -S 1 -B 32",
          "-E 1 -S 128 -B 32");
$dir = "/u/css/classes/3482/154/lab4/";
$instrProg = "/u/css/classes/3482/154/lab4/cacheSim";

if (! -e "cacheSim")
{
   print "missing cacheSim executable\n";
   exit();
}

$pass = 0;

for ($i = 0; $i <= $#files; $i++){
   $file = $dir.$files[$i];
   for ($j = 0; $j <= $#tests; $j++) {
      $input = "-F ".$file." ".$tests[$j];

      #run student simulator on the input file
      #print "Creating $studentoutput \n";
      $studentexecute = "./cacheSim $input > student.output";
      print "$studentexecute\n";
      $instructorexecute = "$instrProg $input > instructor.output";
      system $studentexecute;
      system $instructorexecute;
       
      $diffexecute = "diff student.output instructor.output > student.problems";
      system $diffexecute;
      system "rm student.output instructor.output";
   
      #if there are differences, keep the problem and ourdump files
      if (! system "test -s student.problems"){
         print "Failed.\n";
      } else {
         print "Passed.\n";
         $pass = $pass + 1;
      }
   }
}
$total = ($#files + 1) * ($#tests + 1);
print "$pass out of $total passed.\n";


