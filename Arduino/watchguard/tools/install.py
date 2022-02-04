"""
This script will attempt to install subdirectories located in the source library
path of the project into the local Arduino library path identified by paths.json
file.
"""

import json
import os
import os.path
import sys
import zipfile


PATHS_JSON_FILE = 'paths.json'

SOURCE_DIRECTORY_FIELD_NAME = 'source_library_dir'
DESTINATION_DIRECTORY_FIELD_NAME = 'arduino_library_dir'


def check_field_is_defined(field_name, json_dict):
    if field_name in json_dict:
        return
    sys.exit('Field "{}" in file "{}" is not defined.'.format(
        field_name,
        PATHS_JSON_FILE
    ))


def check_field_is_str(field_name, json_dict):
    if type(json_dict[field_name]) == str:
        return
    sys.exit('Field "{}" in file "{}" is not a valid string.'.format(
        field_name,
        PATHS_JSON_FILE
    ))


def check_field_is_valid_dir(field_name, json_dict):
    dir_path = json_dict[field_name]
    if not os.path.exists(dir_path):
        sys.exit('Path "{}" of field "{}" does not exist.'.format(
            json_dict[field_name],
            field_name
        ))
    if not os.path.isdir(dir_path):
        sys.exit('Path "{}" of field "{}" is not a directory.'.format(
            json_dict[field_name],
            field_name
        ))
    if not os.access(dir_path, os.R_OK | os.W_OK):
        sys.exit('Path "{}" of field "{}" lacks read/write permission.'.format(
            json_dict[field_name],
            field_name
        ))

def zip_directory(directory_path):
    _, dirname = os.path.split(directory_path)
    zip_name = '{}.zip'.format(dirname)
    zip_object = zipfile.ZipFile(zip_name, 'w')
    for root, _, files in os.walk(directory_path):
        for file in files:
            entry_path = os.path.join(root, file)
            zip_object.write(entry_path)
    zip_object.close()
    return zip_name
    
def main():
    """Program entry point."""
    # Read .json file for library source and library destination paths
    try:
        file = open(PATHS_JSON_FILE, 'r')
        data = json.load(file)
        file.close()
    except FileNotFoundError:
        sys.exit('File {} does not exist in directory.'.format(PATHS_JSON_FILE))

    # Check if fields values are valid
    field_checks = [
        check_field_is_defined,
        check_field_is_str,
        check_field_is_valid_dir
    ]

    for check in field_checks:
        check(SOURCE_DIRECTORY_FIELD_NAME, data)
    for check in field_checks:
        check(DESTINATION_DIRECTORY_FIELD_NAME, data)
    
    # Save path names
    source_directory = data[SOURCE_DIRECTORY_FIELD_NAME]
    destination_directory = data[DESTINATION_DIRECTORY_FIELD_NAME]
    original_cwd = os.getcwd()

    # Change directory into library source path and zip libraries
    os.chdir(source_directory)

    zip_files = []
    for subdirectory in os.listdir():
        zip_files.append(zip_directory(subdirectory))

    # Move .zip files from source to destination directory
    for file_name in zip_files:
        os.replace(file_name, os.path.join(destination_directory, file_name))
    
    os.chdir(original_cwd)


if __name__ == '__main__':
    main()