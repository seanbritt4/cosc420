% A = [84 87 78; 16 94 36; 87 93 50;]
% x = [1; 1; 1;]
A=[2 -12; 1 -5;]
x = [1; 1;]


%function retval = power(A, x)
  %D = diag(x);
  %P = A*inv(D);
  %retval = P;
%endfunction

% for i = 0:5
  % x_i = A*x
  % x * x_i
  x_i = A*x
  % x_n = norm(x_i)
  % x_k = x_i / x_n
  % P = M*inv(D
% endfor

% def power_iteration(A, num_simulations):
%     b_k = np.random.rand(A.shape[1])
%
%     for _ in range(num_simulations):
%         # calculate the matrix-by-vector product Ab
%         b_k1 = np.dot(A, b_k)
%
%         # calculate the norm
%         b_k1_norm = np.linalg.norm(b_k1)
%
%         # re normalize the vector
%         b_k = b_k1 / b_k1_norm
%
%     return b_k
%
% power_iteration(np.array([[0.5, 0.5], [0.2, 0.8]]), 10)
