type http_method = number;
type httpd_req_t = pointer;

declare class HttpServer {
	constructor(port: number);
	stop(): void;
	registerUriHandler(uri: string, method: http_method, cb: (req: httpd_req_t, res: HttpServer.Response) => void): void;
	registerUriHandlerWebsocket(uri: string, method: http_method, cb: (socket: HttpServer.Socket) => void): void;
}

declare namespace HttpServer {
	interface Response {
		send(text: string): void;
	}

	interface Socket {
		send(text: string): void;
	}
}

declare const HTTP_DELETE: http_method;
declare const HTTP_GET: http_method;
declare const HTTP_HEAD: http_method;
declare const HTTP_POST: http_method;
declare const HTTP_PUT: http_method;
