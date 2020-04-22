import time


class TimedLoop(object):
    """
    Decorator class to run a function repeatedly with some specified period.
    Use:
    - write a function which is meant to be called on a loop.
    - decorate the function with this class, specifying dt
    - call the function in a while or for loop
    Example:
    # This program will print 'loop_func ran' every .75 seconds
    @TimedLoop(dt=.75)
    def loop_func():
        print('loop_func ran')
    while True:
        loop_func()
    This decorator uses an internal counter in order to properly maintain the
    timing, i.e. slow down if ahead and try to catch up if behind. In order to
    maintain a proper counter while running multiple looping functions, the
    same TimedLoop object must be applied to all functions.
    Example:
    # This program will print 'funcA ran', 'funcB ran', 'funcC ran' with .75
    # second intervals in between.
    TL = TimedLoop(dt=.75)
    @TL2
    def funcA():
        print("funcA ran")
    @TL
    def funcB():
        print("funcB ran")
    @TL
    def funcC():
        print("funcC ran")
    while True:
        funcA()
        funcB()
        funcC()
    """

    def __init__(self, dt):
        self.i = 0
        self.dt = dt
        self.init_time = None

    def __call__(self, loop_func, *args, **kwargs):

        def wrapper(*args, **kwargs):

            # increment call count
            self.i += 1

            try:
                # calculate start time and desired end time
                des_end_time = self.i * self.dt + self.init_time
            except TypeError:  # raised if init_time has not been initialized
                self.init_time = time.time()
                des_end_time = self.i * self.dt + self.init_time

            # call function
            res = loop_func(*args, **kwargs)

            # sleep until desired end time or just continue
            time.sleep(max(des_end_time - time.time(), 0))

            # return result
            return res

        return wrapper

    def change_dt(self, new_dt):
        """
        Change dt for Timed Loop to hit between each sucessive call to its
        wrapped function
        :param new_dt:
        :return:
        """
        self.i = 0
        self.dt = new_dt
        self.init_time = None

    def reset_start_time(self):
        """
        reset start time
        :return:
        """
        self.i = 0
        self.init_time = None
