#avifile("codecs")

codec = "msmpeg4v2"

m = avifile("test.avi", "codec", codec, "fps", 25, "bitrate", 500000)
for i = 1:100
  I = zeros(100,100);
  I(i,:) = i;
  I(:,i) = 200-i;

  for x = 1:100
      I(round(50+10*sin((x+i)/100*4*pi)), x) = 40;
  endfor
  
  addframe(m, I/255)
  printf(".")
endfor
printf("\n")
clear m

m = avifile("test2.avi", "codec", codec)
for i = 1:100
  I = zeros(100,100,3);
  
  for x = 1:100
      I(round(50+10*sin((x+i)/100*4*pi)), x, 1) = 40;
      I(round(50+10*sin((x+i)/100*4*pi)), x, 2) = 40;
      I(round(50+10*sin((x+i)/100*4*pi)), x, 3) = 180;
  endfor
  
  I(i,:,1) = 0;
  I(i,:,2) = 50+i*2;
  I(i,:,3) = 0;
  I(:,i,1) = 200-(i*2);
  I(:,i,2) = 0;
  I(:,i,3) = (i*2);
  
  addframe(m, I/255)
  printf(".")
endfor
printf("\n")
clear m

aviinfo("test.avi")
aviinfo("test2.avi")
y = aviread("test2.avi", 15);
imshow(y(:,:,1), y(:,:,2), y(:,:,3))
