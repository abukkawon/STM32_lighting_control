import serial
import serial.tools.list_ports
from tkinter import *

root = Tk()
root.title("Terminal SM PROJECT ")
root.maxsize(width=700, height=250)
root.minsize(width=700, height=250)
ser = serial.Serial()


def get_ports():
    com_list = serial.tools.list_ports.comports()
    options = []
    for element in com_list:
        options.append(element.device)
    return options


def write(slider, text):
    br, pr, db, port, sb = get_parameters()
    print("Parity: ", pr)
    print("Baud rate: ", br)
    print("Data bits: ", db)
    print("Stop bits: ", sb)
    print("Port: ", port)

    if br == "None":
        _text = "Set baud rate !"
    elif db == "None":
        _text = "Set data bits"
    elif pr == "None":
        _text = "Set parity !"
    elif sb == "None":
        _text = "Set stop bits"
    elif port == "Set PORT":
        _text = "Set Port"
    else:
        ser.baudrate = float(br)
        ser.port = port
        ser.parity = pr
        ser.stopbits = float(sb)
        ser.bytesize = float(db)
        ser.open()
        text_to_send = str(slider.get())
        ser.write(text_to_send)
        _text = "You set value to: %d" % slider.get()
    text.configure(text=_text)


def create_button(slider, text):
    button = Button(root, text="Send value", command=lambda: write(slider, text))
    button['activebackground'] = 'yellow'
    button['width'] = 0
    button.grid(column=3, row=8)


clicked = StringVar(root)


def set_ports(ports):
    clicked.trace("w", get_port)
    clicked.set("Set PORT")
    drop = OptionMenu(root, clicked, ports)
    drop.grid(column=5, row=0, padx=20)


def get_port(*args):
    port = clicked.get()
    return port


baud_rate = StringVar(root)


def baud_rates():
    MODES = [
        ("9600", 9600),
        ("115200", 115200),
    ]

    baud_rate.trace("w", get_baud_rate)
    baud_rate.set("None")  # initialize
    text = Label(root, text="Set baud rate")
    text.grid(column=1, row=0, padx=20)

    i = 1
    for text, mode in MODES:
        b = Radiobutton(root, text=text,
                        variable=baud_rate, value=mode)
        b.grid(column=1, row=i)
        i += 1


def get_baud_rate(*args):
    baud_r = baud_rate.get()
    return baud_r


parity = StringVar(root)


def parity_buttons():
    MODES = [
        ("none", serial.PARITY_NONE),
        ("odd", serial.PARITY_ODD),
        ("even", serial.PARITY_EVEN),
        ("mark", serial.PARITY_MARK),
        ("space", serial.PARITY_SPACE),
    ]

    parity.trace("w", get_parity)
    parity.set("None")  # initialize
    text = Label(root, text="Set parity")
    text.grid(column=3, row=0)

    i = 1
    for text, mode in MODES:
        b = Radiobutton(root, text=text,
                        variable=parity, value=mode)
        b.grid(column=3, row=i)
        i += 1


def get_parity(*args):
    number_of_parity = parity.get()
    return number_of_parity


bits = StringVar()


def data_bits():
    MODES = [
        ("5", serial.FIVEBITS),
        ("6", serial.SIXBITS),
        ("7", serial.SEVENBITS),
        ("8", serial.EIGHTBITS),
    ]
    bits.trace("w", get_data_bits)
    bits.set("None")  # initialize
    text = Label(root, text="Set number of bits")
    text.grid(column=2, row=0)

    i = 1
    for text, mode in MODES:
        b = Radiobutton(root, text=text,
                        variable=bits, value=mode)
        b.grid(column=2, row=i)
        i += 1


def get_data_bits(*args):
    data_bits = bits.get()
    return data_bits


stop_bit = StringVar()


def stop_bits():
    MODES = [
        ("1", serial.STOPBITS_ONE),
        ("1.5", serial.STOPBITS_ONE_POINT_FIVE),
        ("2", serial.STOPBITS_TWO),
    ]
    stop_bit.trace("w", get_stop_bits)
    stop_bit.set("None")  # initialize
    text = Label(root, text="Set number of stop bits")
    text.grid(column=4, row=0)

    i = 1
    for text, mode in MODES:
        b = Radiobutton(root, text=text,
                        variable=stop_bit, value=mode)
        b.grid(column=4, row=i)
        i += 1


def get_stop_bits(*args):
    stop = stop_bit.get()
    return stop


def create_slider(text):
    var = DoubleVar()
    slider = Scale(root, variable=var, orient=HORIZONTAL, width=20, length=400)
    slider.grid(row=7, column=2, columnspan=3, rowspan=1)
    create_button(slider, text)


def get_parameters():
    br = get_baud_rate()
    pr = get_parity()
    db = get_data_bits()
    sb = get_stop_bits()
    port = get_port()
    return br, pr, db, port, sb


def main():
    text = Label(root, text='')
    ports = get_ports()
    baud_rates()
    data_bits()
    stop_bits()
    parity_buttons()
    set_ports(ports)
    create_slider(text)
    text.grid(column=3, row=9)
    root.mainloop()


if __name__ == '__main__':
    main()
