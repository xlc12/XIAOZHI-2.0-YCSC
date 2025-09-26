import sys
import os
from PIL import Image

def gif_to_c_array(gif_file, output_file, variable_name, width, height):
    # 读取GIF文件
    try:
        with open(gif_file, 'rb') as f:
            gif_data = f.read()
    except Exception as e:
        print(f"无法读取GIF文件: {e}")
        return False
    
    # 写入C文件
    try:
        with open(output_file, 'w') as f:
            f.write('// 自动生成的GIF数据\n')
            f.write('// 来源文件: {}\n\n'.format(os.path.basename(gif_file)))
            f.write('#include <lvgl.h>\n\n')
            
            # 写入数据数组
            f.write(f'const uint8_t {variable_name}_data[] = {{\n')
            
            # 格式化输出，每行12个字节
            for i in range(0, len(gif_data), 12):
                chunk = gif_data[i:i+12]
                hex_values = ['0x{:02X}'.format(b) for b in chunk]
                f.write('    ' + ', '.join(hex_values))
                
                if i + 12 < len(gif_data):
                    f.write(',')
                f.write('\n')
            
            f.write(f'}};\n\n')
            
            # 写入图像描述符
            f.write(f'const lv_img_dsc_t {variable_name} = {{\n')
            f.write(f'    .header.flags = LV_IMAGE_FLAGS_ALPHA_8BIT | LV_IMAGE_FLAGS_MODIFIABLE,\n')
            f.write(f'    .header.cf = LV_COLOR_FORMAT_I8,\n')
            f.write(f'    .header.w = {width},\n')
            f.write(f'    .header.h = {height},\n')
            f.write(f'    .data_size = sizeof({variable_name}_data),\n')
            f.write(f'    .data = {variable_name}_data\n')
            f.write(f'}};\n')
        
        print(f"成功生成: {output_file}")
        return True
    except Exception as e:
        print(f"无法写入输出文件: {e}")
        return False

if __name__ == '__main__':
    if len(sys.argv) < 5:
        print("用法: python gif_to_c.py <gif文件> <输出文件> <变量名> <宽度> <高度>")
        print("示例: python gif_to_c.py my_emoji.gif custom_gif_data.c my_emoji_32 32 32")
        sys.exit(1)
    
    gif_file = sys.argv[1]
    output_file = sys.argv[2]
    variable_name = sys.argv[3]
    width = int(sys.argv[4])
    height = int(sys.argv[5])
    
    gif_to_c_array(gif_file, output_file, variable_name, width, height)