# -*- coding:utf-8 -*-
            
import os
import time

def run(file_dir):
    print(file_dir)
    L=[]
    for root, dirs, files in os.walk(file_dir):
        for file in files:
            if os.path.splitext(file)[1] == '.jpg':
                file_name = os.path.splitext(file)[0]
                print(file_name)
                command = './main '+file_name
                os.system(command)
                time.sleep(10)

run('../Dataset/')