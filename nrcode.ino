clc; 
clear; 
close all; 
 
% Step 1: Read and convert the image to grayscale 
I_rgb = imread('https://horseracingnoise.com/wp-content/uploads/2022/11/LRFEATURED
1024x682.png'); 
I_gray = rgb2gray(I_rgb);     % Convert to grayscale 
I = im2double(I_gray);        % Convert to double 
 
% Image size 
[M, N] = size(I); 
 
% Step 2: Add periodic noise (horizontal sinusoidal noise) 
f_noise = 150;  % Integer cycles across the image width 
amplitude = 0.3; 
 
[x, ~] = meshgrid(1:N, 1:M); 
sinusoidal_noise = amplitude * sin(2 * pi * f_noise * x / N);  % Horizontal sinusoidal pattern 
 
% Add noise to image 
I_noisy = I + sinusoidal_noise; 
 
% Step 3: FFT of noisy image 
F = fftshift(fft2(I_noisy)); 
 
% Step 4: Design a symmetric notch reject filter 
D0 = 4; % Notch width 
 
% Initialize filter 
H = ones(M, N); 
 
% Coordinates of noise frequency in FFT 
u0 = round(N/2 + f_noise);     % Horizontal frequency spike 
v0 = round(M/2);               % Vertical center 
 
% Notch filter (Gaussian) at both positive and negative frequency locations 
for u = 1:M 
    for v = 1:N 
        D1 = sqrt((u - v0)^2 + (v - u0)^2);               % Target spike 
        D2 = sqrt((u - v0)^2 + (v - (N - u0 + 1))^2);      % Symmetric spike 
        H(u,v) = H(u,v) * (1 - exp(-(D1^2)/(2*D0^2))) * (1 - exp(-(D2^2)/(2*D0^2))); 
    end 
end 

% Add visible lines at notch locations for visualization 
H_with_lines = H; 
H_with_lines(v0-1:v0+1, :) = 0;  

% Horizontal center line 
H_with_lines(:, u0-1:u0+1) = 0;   

% Vertical at +f_noise 
H_with_lines(:, (N - u0 + 1)-1:(N - u0 + 1)+1) = 0;  % Vertical at -f_noise 

% Step 5: Apply filter in frequency domain 
F_filtered = F .* H; 

% Step 6: Inverse FFT to reconstruct image 
I_filtered = real(ifft2(ifftshift(F_filtered))); 

% Step 7: Display all results 
figure; 
subplot(2,3,1); imshow(I,[]); title('Original Grayscale Image'); 
subplot(2,3,2); imshow(I_noisy,[]); title('Periodic Noise Corrupted Image'); 
subplot(2,3,3); imshow(log(1 + abs(F)),[]); title('FFT of Noisy Image'); 
subplot(2,3,4); imagesc(H_with_lines); colormap gray; axis image; 
title('Notch Filter'); 
subplot(2,3,5); imshow(log(1 + abs(F_filtered)),[]); title('Filtered FFT'); 
subplot(2,3,6); imshow(I_filtered,[]); title('Filtered Image'); 
