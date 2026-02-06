#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


using namespace std;

/**
 * Creates and owns a block of shared memory.
 */
class RLCommunicator {

    string d_name;
    size_t d_size;
    void *d_data;
    
    public:
    RLCommunicator(string const &name, size_t size)
    :
        d_name(name),
        d_size(size),
        d_data(MAP_FAILED)
    {
        int fd;
        void *data;
        try
        {
            // creating the shared memory:
            // https://man7.org/linux/man-pages/man7/shm_overview.7.html
            // https://www.geeksforgeeks.org/linux-unix/posix-shared-memory-api/
            fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
            if (fd == -1)
                throw system_error(
                    errno, generic_category(), "shm_open failed");
            if (ftruncate(fd, size) == -1)
                throw system_error(
                    errno, generic_category(), "ftruncate failed");
            data = mmap(
                nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (data == MAP_FAILED)
                throw system_error(errno, generic_category(), "mmap failed");
            
            // it worked! committing:
            close(fd);
            d_data = data;
        }
        catch(...)
        {   // rolling back:
            close(fd);
            throw;
        }
    }

    ~RLCommunicator() {
        // Safe to do when unlink was already call, it seems :-)
        if (d_data != MAP_FAILED)
            munmap(d_data, d_size);
    }

    void *get_data() {
        return d_data;
    }
    
    /**
     * Unlinks the shared memory.
     */
    void unlink() {
        // According to the docs it doesn't actually isappear until no process
        // uses it anymore. (See example on:
        // https://man7.org/linux/man-pages/man3/shm_unlink.3.html).
        if (shm_unlink(d_name.c_str()) == -1)
            throw system_error(errno, generic_category(), "shm_unlink failed");
    }
};


int main(int argc, char const *argv[])
{
    // Creating shared memory data:
    size_t length = 128;
    RLCommunicator comm{"/godot_python_data", length * sizeof(int64_t)};
    int64_t *data = static_cast<int64_t *>(comm.get_data());
    

    // Setting values for the other to see
    for (size_t idx = 0; idx != length; ++idx)
        data[idx] = idx * idx;
    
    // Waiting:
    for (size_t idx = 0; idx != 10; ++idx) {
        sleep(1);
        cout << "." << flush;
    }
    cout << endl;

    // Reading data the other put in
    for (size_t idx = 0; idx != length; ++idx) {
        if (idx == length / 2)
            cout << endl;
        cout << data[idx] << ", ";
    }
    cout << endl;

    comm.unlink();


    // const char *name = "/godot_python_data";
    // const int length = 128;
    // const int size = length * sizeof(int64_t);
    
    // int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    // if (fd == -1) {
    //     cerr << "Error: could not create shared mem file." << endl;
    //     return 1;
    // }

    // if (ftruncate(fd, size) == -1) {
    //     cerr << "Error: could not truncate memory." << endl;
    //     return 1;
    // }

    // int64_t *mem = static_cast<int64_t *>(
    //     mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    
    // for (int idx = 0; idx != length; ++idx)
    //     mem[idx] = idx * idx;
    
    // for (int idx = 0; idx != 10; ++idx) {
    //     sleep(1);
    //     cout << "." << flush;
    // }
    // cout << endl;

    // for (int idx = 0; idx != length; ++idx)
    //     cout << mem[idx] << ", ";
    // cout << endl;

    // cout << "Unmap returned: " << munmap(mem, size) << endl;
    // close(fd);
    // cout << "Unlink returned: " << shm_unlink(name) << endl;
    
    return 0;
}
