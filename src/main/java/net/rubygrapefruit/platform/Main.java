package net.rubygrapefruit.platform;

public class Main {
    public static void main(String[] args) {
        System.out.println();
        System.out.println("* OS: " + System.getProperty("os.name") + ' ' + System.getProperty("os.version") + ' ' + System.getProperty("os.arch"));

        Process process = Native.get(Process.class);
        System.out.println("* PID: " + process.getProcessId());

        TerminalAccess terminalAccess = Native.get(TerminalAccess.class);
        boolean stdoutIsTerminal = terminalAccess.isTerminal(TerminalAccess.Output.Stdout);
        boolean stderrIsTerminal = terminalAccess.isTerminal(TerminalAccess.Output.Stderr);
        System.out.println("* stdout: " + (stdoutIsTerminal ? "terminal" : "not a terminal"));
        System.out.println("* stderr: " + (stderrIsTerminal ? "terminal" : "not a terminal"));
        if (stdoutIsTerminal) {
            Terminal terminal = terminalAccess.getTerminal(TerminalAccess.Output.Stdout);
            TerminalSize terminalSize = terminal.getTerminalSize();
            System.out.println("* terminal size: " + terminalSize.getCols() + " cols x " + terminalSize.getRows() + " rows");
            System.out.println();
            System.out.println("TERMINAL OUTPUT");
            System.out.print("[normal] ");
            terminal.bold();
            System.out.print("[bold]");
            terminal.normal();
            System.out.println(" [normal]");

            System.out.println();
            System.out.println("COLORS");
            for (Terminal.Color color : Terminal.Color.values()) {
                terminal.foreground(color);
                System.out.print(String.format("[%s] ", color.toString().toLowerCase()));
                terminal.bold();
                System.out.print(String.format("[%s]", color.toString().toLowerCase()));
                terminal.normal();
                System.out.println();
            }

            System.out.println();
            terminal.reset();

            System.out.println("CURSOR MOVEMENT");
            System.out.println("                    ");
            System.out.println("                    ");
            System.out.println("                    ");
            System.out.print("[delete me]");

            terminal.cursorLeft(11);
            terminal.cursorUp(1);
            terminal.cursorRight(10);
            System.out.print("[4]");
            terminal.cursorUp(1);
            terminal.cursorLeft(3);
            System.out.print("[2]");
            terminal.cursorLeft(13);
            System.out.print("[1]");
            terminal.cursorLeft(3);
            terminal.cursorDown(1);
            System.out.print("[3]");
            terminal.cursorDown(1);
            terminal.cursorStartOfLine();
            terminal.clearToEndOfLine();
            System.out.println("done!");
        }

        System.out.println();
    }
}
