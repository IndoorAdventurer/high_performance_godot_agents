#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


using namespace std;

int main(int argc, char const *argv[])
{
    const char *name = "/godot_python_data";
    const int length = 128;
    const int size = length * sizeof(int64_t);
    
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        cerr << "Error: could not create shared mem file." << endl;
        return 1;
    }

    if (ftruncate(fd, size)) {
        cerr << "Error: could not truncate memory." << endl;
        return 1;
    }

    int64_t *mem = static_cast<int64_t *>(
        mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    
    for (int idx = 0; idx != length; ++idx)
        mem[idx] = idx * idx;
    
    for (int idx = 0; idx != 10; ++idx) {
        sleep(1);
        cout << "." << flush;
    }
    cout << endl;

    cout << "Unmap returned: " << munmap(mem, size) << endl;
    close(fd);
    cout << "Unlink returned: " << shm_unlink(name) << endl;
    
    return 0;
}
