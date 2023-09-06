import subprocess

def convert_to_mp4(input_file_path, output_file_path):
    subprocess.run(['ffmpeg', '-i', input_file_path, output_file_path])

# Example usage
convert_to_mp4('Whiplash.mkv', 'Whiplash.mp4')
