namespace model {

	class DataInterpreter {
	public:
		DataInterpreter();
		virtual ~DataInterpreter();

		unsigned Interpret(void *data);
	private:

	};

} //namespace model
