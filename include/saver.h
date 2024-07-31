#ifndef SAVER_H
#define SAVER_H

#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <string>
#include <atomic>

//#include "MyObject.h" // Include your object header
typedef struct {
    std::string filename;
	int type;
	void *obj;
    uint32_t max_size;
}data_object_t;

typedef enum {
    KLL_TYPE,
    FI_TYPE,
    TYPE_MAX
}data_object_type_e;

class Saver {
public:
  // Constructor to specify filename and save interval
  Saver(int interval, std::string class_name);
  ~Saver();

  // Add an object to the queue for saving
  void AddObjectToSave(void *object, int type, const std::string& filename);

  // Start the background thread to save objects from the queue periodically
  void StartSaving();

  // Manual trigger to save all objects in the queue immediately
  void TriggerSave();

  void StopSaving();

#ifndef TEST
private:
#endif
  // Function to be executed in the background thread
  std::string parent_name;
  void SaveLoop();

  std::atomic<bool> exitSaveLoop;
  std::queue<data_object_t *> objects_to_save_;  // Queue of objects to be saved
  int save_interval_;     // Interval between saves in minutes
  std::thread save_thread_;        // Thread object for saving
  std::mutex queue_mutex_;         // Mutex for queue access
  std::condition_variable cv_;     // Condition variable for thread synchronization

  // Replace this function with your actual logic to save the object to a file
  void SaveObjectToFile(data_object_t *object);
};

#endif // SAVER_H

