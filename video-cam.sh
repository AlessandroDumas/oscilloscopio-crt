jack-stdout -L -S 2048 pure_data:output0 pure_data:output1 | ./main | ffmpeg -re -f rawvideo -pixel_format gray16le -video_size 640x512 -i - -f v4l2 -pix_fmt yuyv422 /dev/video10
