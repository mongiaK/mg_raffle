#!/bin/bash

# 指定要处理的目录
directory="/path/to/your/directory"

# 使用关联数组来存储文件的MD5值
declare -A md5_map
declare -A del_map

# 遍历目录中的文件
find "$directory" -type f -print0 | while IFS= read -r -d '' file; do
    # 计算文件的MD5值
    md5=$(md5sum "$file" | awk '{print $1}')
    
    # 检查MD5值是否已经存在，如果不存在，则将文件路径添加到结果中
    if [[ ! -v md5_map["$md5"] ]]; then
        md5_map["$md5"]=$file
    fi
done

# 打印去重后的文件列表
for md5 in "${!md5_map[@]}"; do
    echo "${md5_map[$md5]}"
done


