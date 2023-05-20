
# prende l'input stereo S16_LE da jack
# e lo invia al programma 
# poi disegna il "buffer" a schermo con ffplay

#size=768x512
size=640x512

#jack-stdout -L -S 512 system:capture_1 system:capture_2 | ./main | ffplay -f rawvideo -pixel_format gray16le -video_size $size -i -

#jack-stdout -L -S 2048 pure_data:output0 pure_data:output1 | ./main | ffplay -f rawvideo -pixel_format gray16le -video_size $size -i -

#lineare
#jack-stdout -L -S 2048 pure_data:output0 pure_data:output1 | ./main | ffplay -f rawvideo -pixel_format gray16le -video_size $size -i -

#con gamma
jack-stdout -L -S 2048 pure_data:output0 pure_data:output1 | ./main | ffplay -f rawvideo -pixel_format gray16le -video_size $size -vf eq=gamma=2.8:brightness=-0.055 -i -

#+blur!!
#jack-stdout -L -S 2048 pure_data:output0 pure_data:output1 | ./main | ffplay -f rawvideo -pixel_format gray16le -video_size $size -vf gblur=sigma=1.4:steps=3,eq=gamma=2.2:brightness=-0.04 -i -


