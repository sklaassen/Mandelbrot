clear all;
c= 1+ 0.5j;
Z=0+0j;
for i=1:10
   Z=Z^2;
   Z=Z+c;
   fprintf('Iteration %i: %d %d\n', i,real(Z),imag(Z));
end