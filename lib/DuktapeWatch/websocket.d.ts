declare namespace websocket {
	declare class Client {
		constructor(host: string, port: number, path: string);
		start(): number;
		send(data: string, timeout?: number): number;
		close(timeout?: number): number;
		destroy(): number;
	}
}
