% M = [ 0 1 1 1 1 ; 0 0 1 1 1; 0 0 0 0 0; 0 0 1 0 1; 1 0 0 1 1 ];
% M=[84 87 78; 16 94 36; 87 93 50;]
% M=[1 2; 3 4;]
M=[2 -12; 1 -5;]

T = M;
d = sum(T,1)
% d=d`
D = diag(d)
Dinv = inv(D);

T = M*Dinv

disp(T);

p = ones(size(T, 1), 1)
e = ones(size(T, 1), 1)

s = 0.05;

%Update;
% p = (1-s)*M*p + s*e

% p=(1-s)*M*p + s*e;
% p = p/norm(p)

%run the following to watch p converge
% for i = 0:80000
%     p=(1-s)*M*p + s*e;
%     p = p/norm(p);
% endfor

% p=(1-s)*M*p + s*e;
% p_k = p/norm(p)
