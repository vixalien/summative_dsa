#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_LOGS 20

typedef struct SensorLog {
  int sensor_id;
  float temperature;
  float humidity;
  float pressure;
  float vibration;
  time_t timestamp;
  struct SensorLog *next;
  struct SensorLog *prev;
} SensorLog;

typedef struct {
  SensorLog *head;
  SensorLog *tail;
  SensorLog *current;
  int count;
  int live_mode;
  pthread_mutex_t log_mutex;
} LogSystem;

LogSystem log_system = {NULL, NULL, NULL, 0, 0, PTHREAD_MUTEX_INITIALIZER};


void init_log_system();
void cleanup_log_system();
SensorLog *create_sensor_log(int sensor_id, float temp, float humidity,
                             float pressure, float vibration);
void add_log_to_system(SensorLog *log);
void navigate_next();
void navigate_prev();
void start_live_mode();
void stop_live_mode();
void clear_all_logs();
void display_current_log();
void display_menu();
void *sensor_data_generator(void *arg);
void save_and_exit();
void load_session_state();
void save_session_state();
int get_log_position(SensorLog* target);

int main() {
  printf("=== IoT Gateway Sensor Logging System ===\n");
  printf("Initializing system...\n");
    init_log_system();
  
  load_session_state();
  
  if (log_system.count == 0) {
      printf("No saved session found. Creating sample logs...\n");
      add_log_to_system(create_sensor_log(1, 23.5, 45.2, 1013.25, 0.1));
      add_log_to_system(create_sensor_log(2, 24.1, 47.8, 1012.80, 0.2));
      add_log_to_system(create_sensor_log(3, 22.9, 44.1, 1014.10, 0.05));
      
      log_system.current = log_system.head;
  }

  char command;
  int running = 1;

  while (running) {
    display_current_log();
    display_menu();

    printf("> ");
    scanf(" %c", &command);

    switch (command) {
    case 'n':
      navigate_next();
      break;
    case 'p':
      navigate_prev();
      break;
    case 'y':
      start_live_mode();
      break;
    case 'z':
      stop_live_mode();
      break;
    case 'c':
      clear_all_logs();
      break;
    case 's':
      save_and_exit();
      running = 0;
      break;
    default:
      printf("Invalid command. Please use n, p, y, z, c, or s.\n");
    }
  }

  cleanup_log_system();
  return 0;
}

void init_log_system() {
  log_system.head = NULL;
  log_system.tail = NULL;
  log_system.current = NULL;
  log_system.count = 0;
  log_system.live_mode = 0;
  pthread_mutex_init(&log_system.log_mutex, NULL);
}

void cleanup_log_system() {
  pthread_mutex_lock(&log_system.log_mutex);

  SensorLog *current = log_system.head;
  while (current != NULL) {
    SensorLog *next = current->next;
    free(current);
    current = next;
  }

  pthread_mutex_unlock(&log_system.log_mutex);
  pthread_mutex_destroy(&log_system.log_mutex);
}

SensorLog *create_sensor_log(int sensor_id, float temp, float humidity,
                             float pressure, float vibration) {
  SensorLog *log = (SensorLog *)malloc(sizeof(SensorLog));
  if (!log)
    return NULL;

  log->sensor_id = sensor_id;
  log->temperature = temp;
  log->humidity = humidity;
  log->pressure = pressure;
  log->vibration = vibration;
  log->timestamp = time(NULL);
  log->next = NULL;
  log->prev = NULL;

  return log;
}

void add_log_to_system(SensorLog *log) {
  if (!log)
    return;

  pthread_mutex_lock(&log_system.log_mutex);

    
  if (log_system.count >= MAX_LOGS && log_system.head) {
    SensorLog *old_head = log_system.head;
    log_system.head = old_head->next;
    if (log_system.head) {
      log_system.head->prev = NULL;
    } else {
      log_system.tail = NULL;
    }

        
    if (log_system.current == old_head) {
      log_system.current = log_system.head;
    }

    free(old_head);
    log_system.count--;
  }
    
  if (log_system.tail) {
    log_system.tail->next = log;
    log->prev = log_system.tail;
    log_system.tail = log;
  } else {
        
    log_system.head = log;
    log_system.tail = log;
    log_system.current = log;
  }

  log_system.count++;

  pthread_mutex_unlock(&log_system.log_mutex);
}

void navigate_next() {
  pthread_mutex_lock(&log_system.log_mutex);

  if (log_system.current && log_system.current->next) {
    log_system.current = log_system.current->next;
    printf("Navigated to next log.\n");
  } else {
    printf("Already at the most recent log.\n");
  }

  pthread_mutex_unlock(&log_system.log_mutex);
}

void navigate_prev() {
  pthread_mutex_lock(&log_system.log_mutex);

  if (log_system.current && log_system.current->prev) {
    log_system.current = log_system.current->prev;
    printf("Navigated to previous log.\n");
  } else {
    printf("Already at the oldest log.\n");
  }

  pthread_mutex_unlock(&log_system.log_mutex);
}

void start_live_mode() {
  if (log_system.live_mode) {
    printf("Live mode is already active.\n");
    return;
  }

  log_system.live_mode = 1;
  printf("Live mode activated. Sensor data streaming...\n");

  // Create thread for generating sensor data
  pthread_t sensor_thread;
  pthread_create(&sensor_thread, NULL, sensor_data_generator, NULL);
  pthread_detach(sensor_thread);
}

void stop_live_mode() {
  if (!log_system.live_mode) {
    printf("Live mode is not active.\n");
    return;
  }

  log_system.live_mode = 0;
  printf("Live mode paused.\n");
}

void clear_all_logs() {
  pthread_mutex_lock(&log_system.log_mutex);

  SensorLog *current = log_system.head;
  while (current != NULL) {
    SensorLog *next = current->next;
    free(current);
    current = next;
  }

  log_system.head = NULL;
  log_system.tail = NULL;
  log_system.current = NULL;
  log_system.count = 0;

  pthread_mutex_unlock(&log_system.log_mutex);

  printf("All logs cleared. Memory freed.\n");
}

void display_current_log() {
  pthread_mutex_lock(&log_system.log_mutex);

  printf("\n=== Current Sensor Log ===\n");

  if (log_system.current) {
    struct tm *timeinfo = localtime(&log_system.current->timestamp);
    printf("Log [Position: %d/%d]\n", get_log_position(log_system.current),
           log_system.count);
    printf("Sensor ID: %03d\n", log_system.current->sensor_id);
    printf("Temperature: %.1f°C\n", log_system.current->temperature);
    printf("Humidity: %.1f%%\n", log_system.current->humidity);
    printf("Pressure: %.2f hPa\n", log_system.current->pressure);
    printf("Vibration: %.2f m/s²\n", log_system.current->vibration);
    printf("Timestamp: %s", asctime(timeinfo));
  } else {
    printf("No logs available.\n");
  }

  pthread_mutex_unlock(&log_system.log_mutex);
}

void display_menu() {
  printf("\nCommands:\n");
  printf("(n) Next log | (p) Previous log | (y) Start live | (z) Pause live\n");
  printf("(c) Clear logs | (s) Save and exit\n");
  if (log_system.live_mode) {
    printf("[LIVE MODE ACTIVE]\n");
  }
}

void *sensor_data_generator(void *arg) {
  (void)arg; // Suppress unused parameter warning
int sensor_id = 1;
    
  while (log_system.live_mode) {
        
    float temp = 20.0 + (rand() % 100) / 10.0;
    float humidity = 30.0 + (rand() % 400) / 10.0;
    float pressure = 1000.0 + (rand() % 300) / 10.0;
    float vibration = (rand() % 50) / 100.0;

    SensorLog *new_log =
        create_sensor_log(sensor_id, temp, humidity, pressure, vibration);
    if (new_log) {
      add_log_to_system(new_log);
      printf(
          "\n[NEW DATA] Sensor %03d: T=%.1f°C H=%.1f%% P=%.2fhPa V=%.2fm/s²\n",
          sensor_id, temp, humidity, pressure, vibration);
    }
sensor_id = (sensor_id % 10) + 1;
sleep(2);
  }

  return NULL;
}

void save_and_exit() {
    printf("Saving session state...\n");
    save_session_state();
    printf("System shutting down gracefully.\n");
    printf("Total logs in system: %d\n", log_system.count);
}

void save_session_state() {
    FILE* file = fopen("session_state.txt", "w");
    if (!file) {
        printf("Error: Could not save session state.\n");
        return;
    }
    
    pthread_mutex_lock(&log_system.log_mutex);
    
    int current_pos = get_log_position(log_system.current);
    fprintf(file, "%d %d\n", log_system.count, current_pos);
    
    
    SensorLog* current = log_system.head;
    while (current != NULL) {
        fprintf(file, "%d %.2f %.2f %.2f %.2f %ld\n",
                current->sensor_id,
                current->temperature,
                current->humidity,
                current->pressure,
                current->vibration,
                current->timestamp);
        current = current->next;
    }
    
    pthread_mutex_unlock(&log_system.log_mutex);
    fclose(file);
    printf("Session state saved to session_state.txt\n");
}

void load_session_state() {
    FILE* file = fopen("session_state.txt", "r");
    if (!file) {
        printf("No previous session state found.\n");
        return;
    }
    
    int saved_count, saved_current_pos;
    if (fscanf(file, "%d %d\n", &saved_count, &saved_current_pos) != 2) {
        printf("Error reading session state header.\n");
        fclose(file);
        return;
    }
    
    printf("Loading previous session: %d logs, position %d\n", saved_count, saved_current_pos);
    
    for (int i = 0; i < saved_count; i++) {
        int sensor_id;
        float temperature, humidity, pressure, vibration;
        time_t timestamp;
        
        if (fscanf(file, "%d %f %f %f %f %ld\n",
                   &sensor_id, &temperature, &humidity, &pressure, &vibration, &timestamp) == 6) {
            
            SensorLog* log = create_sensor_log(sensor_id, temperature, humidity, pressure, vibration);
            if (log) {
                log->timestamp = timestamp;
                add_log_to_system(log);
            }
        } else {
            printf("Error reading sensor log %d\n", i + 1);
            break;
        }
    }
    
    if (saved_current_pos > 0 && saved_current_pos <= log_system.count) {
        log_system.current = log_system.head;
        for (int i = 1; i < saved_current_pos && log_system.current; i++) {
            log_system.current = log_system.current->next;
        }
    }
    
    fclose(file);
    printf("Session state loaded successfully.\n");
}


int get_log_position(SensorLog *target) {
  if (!target)
    return 0;

  int position = 1;
  SensorLog *current = log_system.head;

  while (current && current != target) {
    current = current->next;
    position++;
  }

  return current ? position : 0;
}
