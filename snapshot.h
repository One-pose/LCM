#ifndef SNAPSHOT_H_V0_01
#define SNAPSHOT_H_V0_01

/*
* BASE_INCLUDE_IMPLAMINTATION
* Contain base std libs
*/

#ifdef BASE_INCLUDE_IMPLAMINTATION
    #include <iostream>
    #include <fstream>
    #include <filesystem>
    #include <vector>
#endif

#define SNAP_MAX_SIZE 1000

using namespace std::filesystem;

std::string now_time(){

  std::ifstream fl("/proc/driver/rtc");
  std::string line;
  std::string res;
  for (size_t i = 0; i < 2; i++) {
    std::getline(fl, line);
    for (size_t i = 11; i < line.size(); i++) {
      res += line[i];
    }
    res += " ";
  }
  fl.close();
  return res;

}

int snapshot(std::string filename){

  std::fstream file(filename.c_str(), std::ios::in);
  // std::vector<std::pair<size_t, size_t>> snap_lines(1);

  std::vector<std::streampos> snap_lines;

  // Find Snapshots sizes in {snap_lines}
  if (file.is_open()) {
    std::string     line;
    std::streampos  current_pos;
    while (std::getline(file, line)) {
      if (int ret = line.find("Snapshot"); ret > 0) {
        snap_lines.push_back(current_pos);
        current_pos = snap_lines.back();
        std::cout << line << "   Pos: " << current_pos << '\n';
      }
      current_pos = file.tellg();
    }
    file.close();
  }else{
    std::cerr << "Cant create or open .self-docs-snapshots" << '\n';
    file.close();
  }

  // Check total size {snap_lines}
  std::cout << "snap size: " << snap_lines.size() << '\n';
  if (snap_lines.size() >= SNAP_MAX_SIZE) {
    std::string   temp_filename = filename + "temp";
    std::string   line;

    std::fstream  temp_file(temp_filename, std::ios::app);
    file.open(filename, std::ios::out | std::ios::in);

    std::cout << "seekg pos: " << snap_lines[snap_lines.size() / 2] << '\n';
    file.seekg(snap_lines[snap_lines.size() / 2]);
    while (std::getline(file, line)) {
      std::cout << line << '\n';
      temp_file << line << std::endl;
    }
    std::remove(filename.c_str());
    std::rename(temp_filename.c_str(), filename.c_str());
    temp_file.close();
  }

  file.open(filename.c_str(), std::ios::out | std::ios::app);
  file << " Snapshot [ " << now_time() << "] " << "\n";
  for (auto it : recursive_directory_iterator(".")){
    if (is_directory(it.path())) continue;
    if (is_symlink(it.path())){
      file << "Symlink: " << it.path() << "\n";
      continue;
    }
    file << "File: " << file_size(it) << "  Path: " << it.path() << "\n";
  }
  file << "\n";
  file.close();
  return 1;
}

#endif // SNAPSHOT_H_V0_01
