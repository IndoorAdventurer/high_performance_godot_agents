#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>


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
    sem_t *agent_sem = sem_open("/godot_python_agent_sem", 0);
    sem_t *env_sem = sem_open("/godot_python_env_sem", 0);
    
    // Creating shared memory data:
    size_t length = 10;
    RLCommunicator comm{"/godot_python_data", length * sizeof(int64_t)};
    int64_t *data = static_cast<int64_t *>(comm.get_data());
    for (size_t r_idx = 0; r_idx != length; ++r_idx)
        data[r_idx] = 0;
    
    sem_post(agent_sem);
    
    // The main loop:
    for (size_t idx = 0; idx != 50; ++idx) {
        sem_wait(env_sem);
        
        // Reading data for last response:
        for (size_t r_idx = 0; r_idx != length; ++r_idx)
            cout << data[r_idx] << " ";
        cout << endl;

        // Updating the first half of the data:
        for (size_t r_idx = 0; r_idx != length / 2; ++r_idx)
            data[r_idx] += 1;
        
        sem_post(agent_sem);
    }

    // Ending:
    sem_wait(env_sem);
    cout << "Signalling the end!" << endl;
    data[0] = -1;
    sem_post(agent_sem);


    // // Setting values for the other to see
    // for (size_t idx = 0; idx != length; ++idx)
    //     data[idx] = idx * idx;
    
    // sem_post(agent_sem);

    // // Waiting:
    // for (size_t idx = 0; idx != 10; ++idx) {
    //     sleep(1);
    //     cout << "." << flush;
    // }
    // cout << endl;

    // // Reading data the other put in
    // for (size_t idx = 0; idx != length; ++idx) {
    //     if (idx == length / 2)
    //         cout << endl;
    //     cout << data[idx] << ", ";
    // }
    // cout << endl;

    comm.unlink();
    sem_close(agent_sem);
    sem_close(env_sem);
    
    return 0;
}
